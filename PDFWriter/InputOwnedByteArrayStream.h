/*
   Source File : InputOwnedByteArrayStream.h


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
#pragma once

#include "IByteReaderWithPosition.h"
#include "InputByteArrayStream.h"
#include "ByteList.h"

#include <vector>

/*
	IByteReaderWithPosition implementation that owns a copy of the byte array.
	Use when the stream must remain valid regardless of the source buffer's lifetime.
	Relies on the ByteList class to manage the owned byte array.
*/
class InputOwnedByteArrayStream : public IByteReaderWithPosition
{
public:
	InputOwnedByteArrayStream(const IOBasicTypes::Byte* inByteArray,LongFilePositionType inArrayLength);
	InputOwnedByteArrayStream(const std::vector<IOBasicTypes::Byte>& inBuffer);
	virtual ~InputOwnedByteArrayStream(void);

	// IByteReaderWithPosition implementation
	virtual LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);
	virtual void SetPositionFromEnd(LongFilePositionType inOffsetFromEnd);
	virtual LongFilePositionType GetCurrentPosition();

private:

	ByteList mBuffer;
	InputByteArrayStream mStream;

};
