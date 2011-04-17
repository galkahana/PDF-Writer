#include "InputLimitedStream.h"

using namespace IOBasicTypes;

#include <algorithm>

InputLimitedStream::InputLimitedStream(void)
{
	mStream = NULL;
	mMoreToRead = 0;
	mOwnsStream = false;
}

InputLimitedStream::~InputLimitedStream(void)
{
	if(mStream && mOwnsStream)
		delete mStream;
}

InputLimitedStream::InputLimitedStream(IByteReader* inSourceStream,LongFilePositionType inReadLimit,bool inOwnsStream)
{
	Assign(inSourceStream,inReadLimit,inOwnsStream);
}


void InputLimitedStream::Assign(IByteReader* inSourceStream,LongFilePositionType inReadLimit,bool inOwnsStream)
{
	mStream = inSourceStream;
	mMoreToRead = inReadLimit;
	mOwnsStream = inOwnsStream;
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
