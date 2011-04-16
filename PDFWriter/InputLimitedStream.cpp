#include "InputLimitedStream.h"

using namespace IOBasicTypes;

#include <algorithm>

InputLimitedStream::InputLimitedStream(void)
{
	mStream = NULL;
	mMoreToRead = 0;
}

InputLimitedStream::~InputLimitedStream(void)
{
	if(mStream)
		delete mStream;
}

InputLimitedStream::InputLimitedStream(IByteReader* inSourceStream,LongFilePositionType inReadLimit)
{
	Assign(inSourceStream,inReadLimit);
}


void InputLimitedStream::Assign(IByteReader* inSourceStream,LongFilePositionType inReadLimit)
{
	mStream = inSourceStream;
	mMoreToRead = inReadLimit;
}

LongBufferSizeType InputLimitedStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	LongBufferSizeType readBytes = mStream->Read(inBuffer,(LongBufferSizeType)std::min<LongFilePositionType>((LongFilePositionType)inBufferSize,mMoreToRead));

	mMoreToRead -= readBytes;
	return readBytes;
}

bool InputLimitedStream::NotEnded()
{
	return mStream->NotEnded() && mMoreToRead > 0;
}
