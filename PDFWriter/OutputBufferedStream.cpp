/*
   Source File : OutputBufferedStream.cpp


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
#include "OutputBufferedStream.h"

#include <memory.h>

using namespace IOBasicTypes;
using namespace PDFHummus;

OutputBufferedStream::OutputBufferedStream(void)
{
	Initiate(NULL,DEFAULT_BUFFER_SIZE);
}

void OutputBufferedStream::Initiate(IByteWriterWithPosition* inTargetWriter,LongBufferSizeType inBufferSize)
{
	mBufferSize = inBufferSize;
	mBuffer = new Byte[mBufferSize];
	mCurrentBufferIndex = mBuffer;
	mTargetStream = inTargetWriter;
}

OutputBufferedStream::~OutputBufferedStream(void)
{
	Flush();
	delete[] mBuffer;
	delete mTargetStream;
}

OutputBufferedStream::OutputBufferedStream(LongBufferSizeType inBufferSize)
{
	Initiate(NULL,inBufferSize);
}

OutputBufferedStream::OutputBufferedStream(IByteWriterWithPosition* inTargetWriter,LongBufferSizeType inBufferSize)
{
	Initiate(inTargetWriter,inBufferSize);
}

void OutputBufferedStream::Assign(IByteWriterWithPosition* inWriter)
{
	Flush();
	mTargetStream = inWriter;
}

LongBufferSizeType OutputBufferedStream::Write(const Byte* inBuffer,LongBufferSizeType inSize)
{
	if(mTargetStream)
	{
		LongBufferSizeType bytesWritten;

		// if content to write fits in the buffer write to buffer
		if(inSize <= mBufferSize - (mCurrentBufferIndex - mBuffer))
		{
			if(inSize > 0)
			{
				memcpy(mCurrentBufferIndex,inBuffer,inSize);
				mCurrentBufferIndex+=inSize;
			}
			bytesWritten = inSize;
		}
		else
		{
			// if not, flush the buffer. if now won't fit in the buffer write directly to underlying stream
			// all but what size will fit in the buffer - then write to buffer what leftover will fit in.
			LongBufferSizeType bytesToWriteToBuffer = inSize % mBufferSize;
			if (FlushBuffer() != eSuccess)
				return 0;

			bytesWritten = mTargetStream->Write(inBuffer,inSize-bytesToWriteToBuffer);
			if((inSize-bytesToWriteToBuffer == bytesWritten) && bytesToWriteToBuffer > 0) // all well, continue
			{
				memcpy(mCurrentBufferIndex,inBuffer+(inSize-bytesToWriteToBuffer),bytesToWriteToBuffer);
				mCurrentBufferIndex+=bytesToWriteToBuffer;
				bytesWritten+=bytesToWriteToBuffer;
			}
		}
		return bytesWritten;
	}
	else
		return 0;
}


EStatusCode OutputBufferedStream::FlushBuffer()
{
	if(mTargetStream && mCurrentBufferIndex != mBuffer)
	{
		LongBufferSizeType pendingSize = mCurrentBufferIndex - mBuffer;
		LongBufferSizeType writtenSize = mTargetStream->Write(mBuffer,pendingSize);
		mCurrentBufferIndex = mBuffer;
		if (writtenSize != pendingSize)
			return eFailure;
	}
	return eSuccess;
}

EStatusCode OutputBufferedStream::Flush()
{
	EStatusCode status = FlushBuffer();

	// mTargetStream is always owned by this class (deleted in the destructor
	// unless detached via Assign), so finalize it too.
	if(mTargetStream && mTargetStream->Flush() != eSuccess)
		status = eFailure;

	return status;
}

LongFilePositionType OutputBufferedStream::GetCurrentPosition()
{
	return mTargetStream ? mTargetStream->GetCurrentPosition() + (mCurrentBufferIndex - mBuffer):0;
}