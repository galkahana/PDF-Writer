#include "InputByteArrayStream.h"
#include <memory.h>

InputByteArrayStream::InputByteArrayStream()
{
	mByteArray = NULL;
}

InputByteArrayStream::InputByteArrayStream(Byte* inByteArray,LongFilePositionType inArrayLength)
{
	mByteArray = inByteArray;
	mArrayLength = inArrayLength;
	mCurrentPosition = 0;
}

InputByteArrayStream::~InputByteArrayStream(void)
{
}

void InputByteArrayStream::Assign(Byte* inByteArray,LongFilePositionType inArrayLength)
{
	mByteArray = inByteArray;
	mArrayLength = inArrayLength;
	mCurrentPosition = 0;
}

LongBufferSizeType InputByteArrayStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(!mByteArray)
		return 0;

	LongBufferSizeType amountToRead = 
		inBufferSize < (LongBufferSizeType)(mArrayLength-mCurrentPosition) ? 
		inBufferSize : 
		(LongBufferSizeType)(mArrayLength-mCurrentPosition);

	if(amountToRead>0)
		memcpy(inBuffer,mByteArray+mCurrentPosition,amountToRead);
	mCurrentPosition+= amountToRead;
	return amountToRead;
}

bool InputByteArrayStream::NotEnded()
{
	return mByteArray && mCurrentPosition < mArrayLength;

}

void InputByteArrayStream::Skip(LongBufferSizeType inSkipSize)
{
	mCurrentPosition+= inSkipSize < (LongBufferSizeType)mArrayLength-mCurrentPosition ? inSkipSize : mArrayLength-mCurrentPosition;
}

void InputByteArrayStream::SetPosition(LongFilePositionType inOffsetFromStart)
{
	mCurrentPosition = inOffsetFromStart > (LongFilePositionType)mArrayLength ? mArrayLength:inOffsetFromStart;
}

LongFilePositionType InputByteArrayStream::GetCurrentPosition()
{
	return mCurrentPosition;
}
