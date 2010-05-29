#include "BufferedOutputStream.h"

#include <memory.h>

BufferedOutputStream::BufferedOutputStream(void)
{
	Initiate(NULL,DEFAULT_BUFFER_SIZE);
}

void BufferedOutputStream::Initiate(IByteWriterWithPosition* inTargetWriter,LongBufferSizeType inBufferSize)
{
	mBufferSize = inBufferSize;
	mBuffer = new Byte[mBufferSize];
	mCurrentBufferIndex = mBuffer;
	mTargetStream = inTargetWriter;
}

BufferedOutputStream::~BufferedOutputStream(void)
{
	Flush();
	delete[] mBuffer;
	delete mTargetStream;
}

BufferedOutputStream::BufferedOutputStream(LongBufferSizeType inBufferSize)
{
	Initiate(NULL,inBufferSize);
}

BufferedOutputStream::BufferedOutputStream(IByteWriterWithPosition* inTargetWriter,LongBufferSizeType inBufferSize)
{
	Initiate(inTargetWriter,inBufferSize);
}

void BufferedOutputStream::Assign(IByteWriterWithPosition* inWriter)
{
	Flush();
	mTargetStream = inWriter;
}

LongBufferSizeType BufferedOutputStream::Write(const Byte* inBuffer,LongBufferSizeType inSize)
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
			Flush();

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


void BufferedOutputStream::Flush()
{
	if(mTargetStream && mCurrentBufferIndex != mBuffer)
		mTargetStream->Write(mBuffer,mCurrentBufferIndex - mBuffer);
	mCurrentBufferIndex = mBuffer;
}

LongFilePositionType BufferedOutputStream::GetCurrentPosition()
{
	return mTargetStream ? mTargetStream->GetCurrentPosition() + (mCurrentBufferIndex - mBuffer):0;
}