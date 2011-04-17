#include "InputPredictorPNGSubStream.h"

#include "Trace.h"


using namespace IOBasicTypes;

InputPredictorPNGSubStream::InputPredictorPNGSubStream(void)
{
	mSourceStream = NULL;
	mBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;

}

InputPredictorPNGSubStream::~InputPredictorPNGSubStream(void)
{
	delete[] mBuffer;
	delete mSourceStream;
}

InputPredictorPNGSubStream::InputPredictorPNGSubStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns)
{	
	mSourceStream = NULL;
	mBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;

	Assign(inSourceStream,inColumns);
}


LongBufferSizeType InputPredictorPNGSubStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
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
		if(mSourceStream->Read(mBuffer,mBufferSize) != mBufferSize)
		{
			TRACE_LOG("InputPredictorPNGSubStream::Read, problem, expected columns number read. didn't make it");
			readBytes = 0;
			break;
		}
		*mIndex = 0; // so i can use this as "left" value...we don't care about this one...it's just a tag
		mIndex = mBuffer+1; // skip the first tag

		while(mBufferSize > (LongBufferSizeType)(mIndex - mBuffer) && readBytes < inBufferSize)
		{
			DecodeNextByte(inBuffer[readBytes]);
			++readBytes;
		}
	}
	return readBytes;
}

bool InputPredictorPNGSubStream::NotEnded()
{
	return mSourceStream->NotEnded() || (LongBufferSizeType)(mIndex - mBuffer) < mBufferSize;
}

void InputPredictorPNGSubStream::DecodeNextByte(Byte& outDecodedByte)
{
	outDecodedByte = (Byte)((char)mBuffer[mIndex-mBuffer - 1] + (char)*mIndex);

	*mIndex = outDecodedByte; // saving the encoded value back to the buffer,  as "Left" value
	++mIndex;
}

void InputPredictorPNGSubStream::Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns)
{
	mSourceStream = inSourceStream;

	delete[] mBuffer;
	mBufferSize = inColumns + 1;
	mBuffer = new Byte[mBufferSize];
	memset(mBuffer,0,mBufferSize);
	mIndex = mBuffer + mBufferSize;

}
