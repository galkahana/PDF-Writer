/*
   Source File : InputCharStringDecodeStream.cpp


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
#include "InputCharStringDecodeStream.h"

using namespace PDFHummus;


InputCharStringDecodeStream::InputCharStringDecodeStream(IByteReader* inReadFrom,unsigned long inLenIV)
{
	Assign(inReadFrom,inLenIV);
}

InputCharStringDecodeStream::~InputCharStringDecodeStream(void)
{
}

void InputCharStringDecodeStream::Assign(IByteReader* inReadFrom,unsigned long inLenIV)
{
	mReadFrom = inReadFrom;
	InitializeCharStringDecode(inLenIV);
}


static const int CONSTANT_1 = 52845;
static const int CONSTANT_2 = 22719;
static const int RANDOMIZER_INIT = 4330;
static const int RANDOMIZER_MODULU_VAL = 65536;

void InputCharStringDecodeStream::InitializeCharStringDecode(unsigned long inLenIV)
{
	Byte dummyByte;

	mRandomizer = RANDOMIZER_INIT;

	for(unsigned long i=0;i<inLenIV;++i)
		ReadDecodedByte(dummyByte);
}

EStatusCode InputCharStringDecodeStream::ReadDecodedByte(Byte& outByte)
{
	// Initialize before any failure path so callers that ignore the return
	// code (or that read into a buffer slot the caller will dispatch on)
	// never observe stale or uninitialized data.
	outByte = 0;

	Byte buffer;

	if(mReadFrom->Read(&buffer,1) != 1)
		return PDFHummus::eFailure;

	outByte = DecodeByte(buffer);
	return PDFHummus::eSuccess;
}

Byte InputCharStringDecodeStream::DecodeByte(Byte inByteToDecode)
{
	Byte result = (Byte)(inByteToDecode ^ (mRandomizer >> 8));
	mRandomizer = (unsigned short)(((inByteToDecode + mRandomizer)* CONSTANT_1 + CONSTANT_2) % RANDOMIZER_MODULU_VAL);
	return result;
}

LongBufferSizeType InputCharStringDecodeStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	LongBufferSizeType bufferIndex = 0;

	while(NotEnded() && inBufferSize > bufferIndex)
	{
		// Only count the slot as written when the underlying read succeeded;
		// otherwise the caller would dispatch on an uninitialized byte as
		// if it were a real CharString opcode.
		if(ReadDecodedByte(inBuffer[bufferIndex]) != PDFHummus::eSuccess)
			break;
		++bufferIndex;
	}
	return bufferIndex;
}

bool InputCharStringDecodeStream::NotEnded()
{
	return mReadFrom->NotEnded();
}


