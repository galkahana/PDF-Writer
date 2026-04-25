/*
   Source File : InputPredictorPNGOptimumStream.cpp


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
#include "InputPredictorPNGOptimumStream.h"

#include "Trace.h"
#include <stdlib.h>
#include <stdint.h>

/*
	Note from Gal: Note that optimum also implements the others. this is because PNG compression requires that the first byte in the line holds the algo -
	even if the whole stream is declared as a single algo.
*/

using namespace IOBasicTypes;

InputPredictorPNGOptimumStream::InputPredictorPNGOptimumStream(void)
{
	mSourceStream = NULL;
	mBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;
	mUpValues = NULL;
	mBytesPerPixel = 0;

}

InputPredictorPNGOptimumStream::~InputPredictorPNGOptimumStream(void)
{
	delete[] mBuffer;
	delete[] mUpValues;
	delete mSourceStream;
}

InputPredictorPNGOptimumStream::InputPredictorPNGOptimumStream(IByteReader* inSourceStream,
                                                               IOBasicTypes::LongBufferSizeType inColors,
                                                               IOBasicTypes::Byte inBitsPerComponent,
                                                               IOBasicTypes::LongBufferSizeType inColumns)
{
	mSourceStream = NULL;
	mBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;
	mUpValues = NULL;
	mBytesPerPixel = 0;

	Assign(inSourceStream,inColors,inBitsPerComponent,inColumns);
}


LongBufferSizeType InputPredictorPNGOptimumStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(!mSourceStream || !mBuffer)
		return 0;

	LongBufferSizeType readBytes = 0;


	// exhaust what's in the buffer currently
	while(mBufferSize > (LongBufferSizeType)(mIndex - mBuffer) && readBytes < inBufferSize)
	{
		DecodeNextByte(inBuffer[readBytes]);
		++readBytes;
	}

	// now repeatedly read bytes from the input stream, and decode
	while(readBytes < inBufferSize && mSourceStream->NotEnded())
	{
		memcpy(mUpValues,mBuffer,mBufferSize);
		LongBufferSizeType readFromSource = mSourceStream->Read(mBuffer, mBufferSize);
		if (readFromSource == 0) {
			break; // a belated end. must be flate
		}
		if(readFromSource != mBufferSize)
		{
			TRACE_LOG("InputPredictorPNGOptimumStream::Read, problem, expected columns number read. didn't make it");
			break;
		}
		mFunctionType = *mBuffer;
		*mBuffer = 0; // so i can use this as "left" value...we don't care about this one...it's just a tag
		mIndex = mBuffer+1; // skip the first tag

		while(mBufferSize > (LongBufferSizeType)(mIndex - mBuffer) && readBytes < inBufferSize)
		{
			DecodeNextByte(inBuffer[readBytes]);
			++readBytes;
		}
	}
	return readBytes;
}

bool InputPredictorPNGOptimumStream::NotEnded()
{
	if(!mSourceStream || !mBuffer)
		return false;
	return mSourceStream->NotEnded() || (LongBufferSizeType)(mIndex - mBuffer) < mBufferSize;
}

void InputPredictorPNGOptimumStream::DecodeNextByte(Byte& outDecodedByte)
{
	LongBufferSizeType pos = (LongBufferSizeType)(mIndex - mBuffer);

	// decoding function is determined by mFunctionType
	switch(mFunctionType)
	{
		case 0:
			outDecodedByte = *mIndex;
			break;
		case 1:
		{
			// Per PNG spec, corresponding byte is mBytesPerPixel back, 0 before scanline start
			Byte leftValue = (pos > mBytesPerPixel) ? *(mIndex - mBytesPerPixel) : 0;
			outDecodedByte = (Byte)((char)leftValue + (char)*mIndex);
			break;
		}
		case 2:
			outDecodedByte = (Byte)((char)mUpValues[pos] + (char)*mIndex);
			break;
		case 3:
		{
			// Per PNG spec, corresponding byte is mBytesPerPixel back, 0 before scanline start
			char leftVal = (pos > mBytesPerPixel) ? (char)mBuffer[pos - mBytesPerPixel] : 0;
			outDecodedByte = (Byte)(leftVal/2 + (char)mUpValues[pos]/2 + (char)*mIndex);
			break;
		}
		case 4:
		{
			// Per PNG spec, corresponding byte is mBytesPerPixel back, 0 before scanline start
			char leftVal = (pos > mBytesPerPixel) ? (char)mBuffer[pos - mBytesPerPixel] : 0;
			char upLeftVal = (pos > mBytesPerPixel) ? (char)mUpValues[pos - mBytesPerPixel] : 0;
			outDecodedByte = (Byte)(PaethPredictor(leftVal,(char)mUpValues[pos],upLeftVal) + (char)*mIndex);
			break;
		}
	}

	*mIndex = outDecodedByte; // saving the encoded value back to the buffer, for later copying as "Up value", and current using as "Left" value
	++mIndex;
}

void InputPredictorPNGOptimumStream::Assign(IByteReader* inSourceStream,
											IOBasicTypes::LongBufferSizeType inColors,
											IOBasicTypes::Byte inBitsPerComponent,
											IOBasicTypes::LongBufferSizeType inColumns)
{
	delete[] mBuffer;
	delete[] mUpValues;
	mBuffer = NULL;
	mUpValues = NULL;
	mBufferSize = 0;
	mIndex = NULL;
	mFunctionType = 0;
	mBytesPerPixel = 0;

	// Validate inputs to prevent overflow in buffer size calculation
	if(inColumns == 0 || inColors == 0 || inBitsPerComponent == 0)
	{
		TRACE_LOG("InputPredictorPNGOptimumStream::Assign, invalid zero parameter");
		return;
	}

	// Check multiplication overflow: inColumns * inColors
	if(inColumns > SIZE_MAX / inColors)
	{
		TRACE_LOG("InputPredictorPNGOptimumStream::Assign, overflow in buffer size calculation");
		return;
	}
	LongBufferSizeType colorsTimesColumns = inColumns * inColors;

	// Check next multiplication: colorsTimesColumns * inBitsPerComponent
	if(colorsTimesColumns > SIZE_MAX / inBitsPerComponent)
	{
		TRACE_LOG("InputPredictorPNGOptimumStream::Assign, overflow in buffer size calculation");
		return;
	}
	LongBufferSizeType totalBits = colorsTimesColumns * inBitsPerComponent;

	// Check that +7 won't overflow (extremely unlikely but be thorough)
	if(totalBits > SIZE_MAX - 8)
	{
		TRACE_LOG("InputPredictorPNGOptimumStream::Assign, overflow in buffer size calculation");
		return;
	}

	// All validation passed - now update member state
	mSourceStream = inSourceStream;

	mBytesPerPixel = (inColors * inBitsPerComponent + 7) / 8;
	if(mBytesPerPixel == 0)
		mBytesPerPixel = 1;

	// Rows may contain empty bits at end
	mBufferSize = (totalBits + 7) / 8 + 1;
	mBuffer = new Byte[mBufferSize];
	memset(mBuffer,0,mBufferSize);
	mUpValues = new Byte[mBufferSize];
	memset(mUpValues,0,mBufferSize); // that's less important
	mIndex = mBuffer + mBufferSize;
}

char InputPredictorPNGOptimumStream::PaethPredictor(char inLeft,char inUp,char inUpLeft)
{
	int p = inLeft + inUp - inUpLeft;
	int pLeft = abs(p - inLeft);
	int pUp = abs(p - inUp);
	int pUpLeft = abs(p - inUpLeft);

	if(pLeft <= pUp && pLeft <= pUpLeft)
	  return inLeft;
	else if(pUp <= pUpLeft)
	  return inUp;
	else
	  return inUpLeft;
}
