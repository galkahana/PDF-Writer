#include "InputStringBufferStream.h"
#include "SafeBufferMacrosDefs.h"

InputStringBufferStream::InputStringBufferStream(MyStringBuf* inBufferToReadFrom)
{
	mBufferToReadFrom = inBufferToReadFrom;
}

void InputStringBufferStream::Assign(MyStringBuf* inBufferToReadFrom)
{
	mBufferToReadFrom = inBufferToReadFrom;
}

InputStringBufferStream::~InputStringBufferStream(void)
{
}

LongBufferSizeType InputStringBufferStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	return mBufferToReadFrom->SAFE_SGETN((char*)inBuffer,inBufferSize,inBufferSize);
}

bool InputStringBufferStream::NotEnded()
{
	return mBufferToReadFrom->in_avail() != 0;
}

void InputStringBufferStream::Skip(LongBufferSizeType inSkipSize)
{
	mBufferToReadFrom->pubseekoff(inSkipSize,ios_base::cur);
}

void InputStringBufferStream::SetPosition(LongFilePositionType inOffsetFromStart)
{
	mBufferToReadFrom->pubseekoff((long)inOffsetFromStart,ios_base::beg);
}

LongFilePositionType InputStringBufferStream::GetCurrentPosition()
{
	return mBufferToReadFrom->GetCurrentReadPosition();
}