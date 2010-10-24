#include "OutputStringBufferStream.h"

using namespace IOBasicTypes;

OutputStringBufferStream::OutputStringBufferStream(void)
{
	mBuffer = new MyStringBuf();
	mOwnsBuffer = true;
}

OutputStringBufferStream::~OutputStringBufferStream(void)
{
	if(mOwnsBuffer)
		delete mBuffer;
}

OutputStringBufferStream::OutputStringBufferStream(MyStringBuf* inControlledBuffer)
{
	mBuffer = inControlledBuffer;
	mOwnsBuffer = false;
}

void OutputStringBufferStream::Assign(MyStringBuf* inControlledBuffer)
{
	if(inControlledBuffer)
	{
		if(mOwnsBuffer)
			delete mBuffer;
		mBuffer = inControlledBuffer;
		mOwnsBuffer = false;
	}
	else // passing null will cause the stream to get to a state of self writing (so as to never leave this stream in a vulnarable position)s
	{
		mBuffer = new MyStringBuf();
		mOwnsBuffer = true;
	}

}


LongBufferSizeType OutputStringBufferStream::Write(const Byte* inBuffer,LongBufferSizeType inSize)
{
	return mBuffer->sputn((const char*)inBuffer,inSize);
}

LongFilePositionType OutputStringBufferStream::GetCurrentPosition()
{
	return mBuffer->GetCurrentWritePosition();
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

void OutputStringBufferStream::SetPosition(LongFilePositionType inOffsetFromStart)
{
	mBuffer->pubseekoff((long)inOffsetFromStart,ios_base::beg);
}