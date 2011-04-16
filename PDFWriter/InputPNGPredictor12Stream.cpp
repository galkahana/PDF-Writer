#include "InputPNGPredictor12Stream.h"
#include "Trace.h"


using namespace IOBasicTypes;

InputPNGPredictor12Stream::InputPNGPredictor12Stream(void)
{
	mSourceStream = NULL;
	mBuffer = NULL;
	mPreviousBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;
}

InputPNGPredictor12Stream::~InputPNGPredictor12Stream(void)
{
	delete[] mBuffer;
	delete[] mPreviousBuffer;
}

InputPNGPredictor12Stream::InputPNGPredictor12Stream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns)
{
	mSourceStream = NULL;
	mBuffer = NULL;
	mPreviousBuffer = NULL;
	mIndex = NULL;
	mBufferSize = 0;

	Assign(inSourceStream,inColumns);
}


LongBufferSizeType InputPNGPredictor12Stream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	LongBufferSizeType readBytes = 0;
	

	// exhaust what's in the buffer currently
	while(mIndex != NULL && mBufferSize > (LongBufferSizeType)(mIndex - mBuffer) && readBytes < inBufferSize)
	{
		DecodeNextByte(inBuffer[readBytes]);
		++readBytes;
	}

	// now repeatedly read bytes from the input stream, and decode
	while(readBytes < inBufferSize && mSourceStream->NotEnded())
	{
		if(mBuffer)
		{
			if(!mPreviousBuffer)
			{
				mPreviousBuffer = new Byte[mBufferSize];
				memcpy(mPreviousBuffer,mBuffer,mBufferSize);
			}
			else
			{
				for(LongBufferSizeType i=0;i<mBufferSize;++i)
					mPreviousBuffer[i] = (Byte)((char)mPreviousBuffer[i] + (char)mBuffer[i]);
			}
		}
		else
		{
			mBuffer = new Byte[mBufferSize];
		}
		if(mSourceStream->Read(mBuffer,mBufferSize) != mBufferSize)
		{
			TRACE_LOG("InputPNGPredictor12Stream::Read, problem, expected columns number read. didn't make it");
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

void InputPNGPredictor12Stream::DecodeNextByte(Byte& outDecodedByte)
{
	if(mPreviousBuffer)
		outDecodedByte = (Byte)((char)mPreviousBuffer[mIndex-mBuffer] + (char)*mIndex);
	else
		outDecodedByte = *mIndex;
	++mIndex;
}

bool InputPNGPredictor12Stream::NotEnded()
{
	return mSourceStream->NotEnded() || (LongBufferSizeType)(mIndex - mBuffer) < mBufferSize;
}

void InputPNGPredictor12Stream::Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns)
{
	mSourceStream = inSourceStream;

	delete[] mBuffer;
	delete[] mPreviousBuffer;
	mBufferSize = inColumns + 1;
	mBuffer = NULL;
	mPreviousBuffer = NULL;
	mIndex = NULL;
}
