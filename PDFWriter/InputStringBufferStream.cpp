#include "InputStringBufferStream.h"
#include "SafeBufferMacrosDefs.h"

InputStringBufferStream::InputStringBufferStream(stringbuf* inBufferToReadFrom)
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
