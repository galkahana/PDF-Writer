#include "OutputStringBufferStream.h"

OutputStringBufferStream::OutputStringBufferStream(void)
{
	mBuffer = new stringbuf();
	mOwnsBuffer = true;
}

OutputStringBufferStream::~OutputStringBufferStream(void)
{
	if(mOwnsBuffer)
		delete mBuffer;
}

OutputStringBufferStream::OutputStringBufferStream(stringbuf* inControlledBuffer)
{
	mBuffer = inControlledBuffer;
	mOwnsBuffer = false;
}

LongBufferSizeType OutputStringBufferStream::Write(const Byte* inBuffer,LongBufferSizeType inSize)
{
	return mBuffer->sputn((const char*)inBuffer,inSize);
}

LongFilePositionType OutputStringBufferStream::GetCurrentPosition()
{
	return mBuffer->in_avail();
}

string OutputStringBufferStream::ToString() const
{
	return mBuffer->str();
}

static const string scEmpty;

void OutputStringBufferStream::Reset()
{
	mBuffer->str(scEmpty);
}
