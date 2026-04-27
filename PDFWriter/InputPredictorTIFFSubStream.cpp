/*
   Source File : InputPredictorTIFFSubStream.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "InputPredictorTIFFSubStream.h"
#include "Trace.h"
#include <stdint.h>

using namespace IOBasicTypes;

InputPredictorTIFFSubStream::InputPredictorTIFFSubStream(void)
{
	mSourceStream = NULL;
	mRowBuffer = NULL;
	mReadColors = NULL;
	mReadColorsCount = 0;
}

InputPredictorTIFFSubStream::InputPredictorTIFFSubStream(IByteReader* inSourceStream,
														LongBufferSizeType inColors,
														Byte inBitsPerComponent,
														LongBufferSizeType inColumns)
{
	mSourceStream = NULL;
	mRowBuffer = NULL;
	mReadColors = NULL;

	Assign(inSourceStream,inColors,inBitsPerComponent,inColumns);
}

InputPredictorTIFFSubStream::~InputPredictorTIFFSubStream(void)
{
	delete mSourceStream;
	delete[] mRowBuffer;
	delete[] mReadColors;
}

LongBufferSizeType InputPredictorTIFFSubStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(!mSourceStream)
		return 0;

	LongBufferSizeType readBytes = 0;

	// exhaust what's in the buffer currently
	while(mReadColorsCount > (LongBufferSizeType)(mReadColorsIndex - mReadColors) && readBytes < inBufferSize)
	{
		ReadByteFromColorsArray(inBuffer[readBytes]);
		++readBytes;
	}

	// now repeatedly read bytes from the input stream, and decode
	while(readBytes < inBufferSize && mSourceStream->NotEnded())
	{
		if(mSourceStream->Read(mRowBuffer,(mColumns*mColors*mBitsPerComponent)/8) != (mColumns*mColors*mBitsPerComponent)/8)
		{
			TRACE_LOG("InputPredictorPNGSubStream::Read, problem, expected columns*colors*bitspercomponent/8 number read. didn't make it");
			readBytes = 0;
			break;
		}
		DecodeBufferToColors();

		while(mReadColorsCount > (LongBufferSizeType)(mReadColorsIndex - mReadColors) && readBytes < inBufferSize)
		{
			ReadByteFromColorsArray(inBuffer[readBytes]);
			++readBytes;
		}
	}
	return readBytes;	
}

bool InputPredictorTIFFSubStream::NotEnded()
{
	if(!mSourceStream)
		return false;
	return mSourceStream->NotEnded() || (LongBufferSizeType)(mReadColorsIndex - mReadColors) < mReadColorsCount;
}

void InputPredictorTIFFSubStream::Assign(IByteReader* inSourceStream,
										LongBufferSizeType inColors,
										Byte inBitsPerComponent,
										LongBufferSizeType inColumns)
{
	// Always take ownership of the source stream first
	delete mSourceStream;
	mSourceStream = inSourceStream;

	// Clean up previous buffers
	delete[] mRowBuffer;
	mRowBuffer = NULL;
	delete[] mReadColors;
	mReadColors = NULL;
	mReadColorsCount = 0;

	// Validate inputs
	if(inColumns == 0 || inColors == 0 || inBitsPerComponent == 0)
	{
		if(inSourceStream != NULL)
			TRACE_LOG("InputPredictorTIFFSubStream::Assign, invalid zero parameter");
		return;
	}

	// Check multiplication overflow: inColumns * inColors
	if(inColumns > SIZE_MAX / inColors)
	{
		TRACE_LOG("InputPredictorTIFFSubStream::Assign, overflow in columns * colors");
		return;
	}
	LongBufferSizeType colorsTimesColumns = inColumns * inColors;

	// Check next multiplication: colorsTimesColumns * inBitsPerComponent
	if(colorsTimesColumns > SIZE_MAX / inBitsPerComponent)
	{
		TRACE_LOG("InputPredictorTIFFSubStream::Assign, overflow in buffer size calculation");
		return;
	}
	LongBufferSizeType totalBits = colorsTimesColumns * inBitsPerComponent;

	// Check that +7 won't overflow before ceiling division
	if(totalBits > SIZE_MAX - 7)
	{
		TRACE_LOG("InputPredictorTIFFSubStream::Assign, overflow in buffer size rounding");
		return;
	}
	LongBufferSizeType bufferSize = (totalBits + 7) / 8;

	// All validation passed - update remaining member state
	mColors = inColors;
	mBitsPerComponent = inBitsPerComponent;
	mColumns = inColumns;

	mRowBuffer = new Byte[bufferSize];

	mReadColorsCount = colorsTimesColumns;
	// value-initialize so any cell not written by DecodeBufferToColors (e.g. when a
	// defensive bounds check trips on a crafted input) reads back as 0 instead of
	// uninitialized heap memory.
	mReadColors = new unsigned short[mReadColorsCount]();
	mReadColorsIndex = mReadColors + mReadColorsCount; // assign to end of array so will know that should read new buffer
	mIndexInColor = 0;

	mBitMask = 0;
	for(Byte i=0;i<inBitsPerComponent;++i)
		mBitMask = (mBitMask<<1) + 1;
}

void InputPredictorTIFFSubStream::ReadByteFromColorsArray(Byte& outBuffer)
{
	if(8 == mBitsPerComponent)
	{
		outBuffer = (Byte)(*mReadColorsIndex);
		++mReadColorsIndex;
	}
	else if(8 > mBitsPerComponent)
	{
		outBuffer = 0;
		for(Byte i=0;i<(8/mBitsPerComponent);++i)
		{
			outBuffer = (outBuffer<<mBitsPerComponent) + (Byte)(*mReadColorsIndex);
			++mReadColorsIndex;
		}
	}
	else // 8 < mBitsPerComponent [which is just 16 for now]
	{
		outBuffer =(Byte)(((*mReadColorsIndex)>>(mBitsPerComponent - mIndexInColor*8)) & 0xff);
		++mIndexInColor;
		if(mBitsPerComponent/8 == mIndexInColor)
		{
			++mReadColorsIndex;
			mIndexInColor = 0;
		}
	}
}

void InputPredictorTIFFSubStream::DecodeBufferToColors()
{	
	//1. Split to colors. Use array of colors (should be columns * colors). Each time take BitsPerComponent of the buffer
	//2. Once you got the "colors", loop the array, setting values after diffs (use modulo of bit mask for "sign" computing)
	//3. Now you have the colors array. 
	LongBufferSizeType i = 0;

	// read the colors differences according to bits per component
	if(8 == mBitsPerComponent)
	{
		for(; i < mReadColorsCount;++i)
			mReadColors[i] = mRowBuffer[i];
	}
	else if(8 > mBitsPerComponent)
	{
		for(; i < (mReadColorsCount*mBitsPerComponent/8);++i)
		{
			for(LongBufferSizeType j=0; j < (LongBufferSizeType)(8/mBitsPerComponent); ++j)
			{
				LongBufferSizeType writeIndex = (i+1)*8/mBitsPerComponent - j - 1;
				// defensive bounds check: with non-power-of-two BitsPerComponent or
				// counts that don't divide evenly, the computed index could in principle
				// land outside the mReadColors array. fall through gracefully rather
				// than write past the buffer; unfilled cells were zero-initialized in
				// Assign(), so downstream reads see 0 instead of uninitialized memory.
				if(writeIndex >= mReadColorsCount)
				{
					TRACE_LOG3("InputPredictorTIFFSubStream::DecodeBufferToColors, write index %lu out of bounds (count=%lu, bpc=%u), skipping",
						(unsigned long)writeIndex, (unsigned long)mReadColorsCount, (unsigned)mBitsPerComponent);
					break;
				}
				mReadColors[writeIndex] = mRowBuffer[i] & mBitMask;
				mRowBuffer[i] = mRowBuffer[i]>>mBitsPerComponent;
			}
		}
	}
	else // mBitesPerComponent > 8
	{
		for(; i < mReadColorsCount;++i)
		{
			mReadColors[i] = 0;
			for(Byte j=0;j<mBitsPerComponent/8;++j)
				mReadColors[i] = (mReadColors[i]<<mBitsPerComponent) + mRowBuffer[i*mBitsPerComponent/8 + j];
		}
	}

	// calculate color values according to diffs
	for(i = mColors; i < mReadColorsCount; ++i)
		mReadColors[i] = (mReadColors[i] + mReadColors[i-mColors]) & mBitMask;

	mReadColorsIndex = mReadColors;
	mIndexInColor = 0;

}