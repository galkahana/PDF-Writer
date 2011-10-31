/*
   Source File : InputByteArrayStream.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


*/
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

void InputByteArrayStream::Assign(IOBasicTypes::Byte* inByteArray,IOBasicTypes::LongFilePositionType inArrayLength)
{
	mByteArray = inByteArray;
	mArrayLength = inArrayLength;
	mCurrentPosition = 0;
}

LongBufferSizeType InputByteArrayStream::Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize)
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
	mCurrentPosition = inOffsetFromStart > mArrayLength ? mArrayLength:inOffsetFromStart;
}

void InputByteArrayStream::SetPositionFromEnd(LongFilePositionType inOffsetFromEnd)
{
	mCurrentPosition = inOffsetFromEnd > mArrayLength ? 0:(mArrayLength-inOffsetFromEnd);
}

LongFilePositionType InputByteArrayStream::GetCurrentPosition()
{
	return mCurrentPosition;
}
