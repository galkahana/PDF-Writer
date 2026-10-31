/*
   Source File : InputOwnedByteArrayStream.cpp


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
#include "InputOwnedByteArrayStream.h"

using namespace IOBasicTypes;

InputOwnedByteArrayStream::InputOwnedByteArrayStream(const Byte* inByteArray,LongFilePositionType inArrayLength)
	: mBuffer(inByteArray,inArrayLength)
{
	mStream.Assign(mBuffer.data(),inArrayLength);
}

InputOwnedByteArrayStream::InputOwnedByteArrayStream(const std::vector<Byte>& inBuffer)
	: mBuffer(inBuffer.empty() ? NULL : &inBuffer[0],inBuffer.size())
{
	mStream.Assign(mBuffer.data(),(LongFilePositionType)inBuffer.size());
}

InputOwnedByteArrayStream::~InputOwnedByteArrayStream(void)
{
}

LongBufferSizeType InputOwnedByteArrayStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	return mStream.Read(inBuffer,inBufferSize);
}

bool InputOwnedByteArrayStream::NotEnded()
{
	return mStream.NotEnded();
}

void InputOwnedByteArrayStream::Skip(LongBufferSizeType inSkipSize)
{
	mStream.Skip(inSkipSize);
}

void InputOwnedByteArrayStream::SetPosition(LongFilePositionType inOffsetFromStart)
{
	mStream.SetPosition(inOffsetFromStart);
}

void InputOwnedByteArrayStream::SetPositionFromEnd(LongFilePositionType inOffsetFromEnd)
{
	mStream.SetPositionFromEnd(inOffsetFromEnd);
}

LongFilePositionType InputOwnedByteArrayStream::GetCurrentPosition()
{
	return mStream.GetCurrentPosition();
}
