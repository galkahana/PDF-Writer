#include "InputPredictorPNGNoneStream.h"

#include "Trace.h"


using namespace IOBasicTypes;

InputPredictorPNGNoneStream::InputPredictorPNGNoneStream(void)
{
	mSourceStream = NULL;
	mBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;
}

InputPredictorPNGNoneStream::~InputPredictorPNGNoneStream(void)
{
	delete[] mBuffer;
	delete mSourceStream;
}

InputPredictorPNGNoneStream::InputPredictorPNGNoneStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns)
{	
	mSourceStream = NULL;
	mBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;

	Assign(inSourceStream,inColumns);
}


LongBufferSizeType InputPredictorPNGNoneStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
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
			TRACE_LOG("InputPredictorPNGNoneStream::Read, problem, expected columns number read. didn't make it");
			readBytes = 0;
			break;
		}
		mIndex = mBuffer+1; // skip the first tag

		while(mBufferSize > (LongBufferSizeType)(mIndex - mBuffer) && readBytes < inBufferSize)
		{
			DecodeNextByte(inBuffer[readBytes]);
			++readBytes;
		}
	}
	return readBytes;
}

bool InputPredictorPNGNoneStream::NotEnded()
{
	return mSourceStream->NotEnded() || (LongBufferSizeType)(mIndex - mBuffer) < mBufferSize;
}

void InputPredictorPNGNoneStream::DecodeNextByte(Byte& outDecodedByte)
{
	outDecodedByte = *mIndex;

	++mIndex;
}

void InputPredictorPNGNoneStream::Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns)
{
	mSourceStream = inSourceStream;

	delete[] mBuffer;
	mBufferSize = inColumns + 1;
	mBuffer = new Byte[mBufferSize];
	mIndex = mBuffer + mBufferSize;

}
