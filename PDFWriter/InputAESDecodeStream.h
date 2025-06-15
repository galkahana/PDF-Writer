/*
   Source File : InputAESDecodeStream.h


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

#include "EStatusCode.h"
#include "IByteReader.h"
#include "aescpp.h"

#include <string>

typedef std::basic_string<IOBasicTypes::Byte> ByteList;

class InputAESDecodeStream : public IByteReader
{
public:
	InputAESDecodeStream(void);
	~InputAESDecodeStream(void);

	// InputAESDecodeStream owns inSourceReader and will delete it on destruction.
	InputAESDecodeStream(
		IByteReader* inSourceReader, 
		const ByteList& inKey); // inKey list length can be anything the underlying AES implemenetaion supports, specifically AES-128, AES-256, etc.

	// IByteReader implementation. note that "inBufferSize" determines how many
	// bytes will be placed in the Buffer...not how many are actually read from the underlying
	// encoded stream. got it?!
	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer, IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

private:

	// inEncryptionKey in array form, for aes
	unsigned char* mKey;
	std::size_t  mKeyLength;
	unsigned char mIV[AES_BLOCK_SIZE];
	unsigned char mIn[AES_BLOCK_SIZE];
	unsigned char mInNext[AES_BLOCK_SIZE];
	unsigned char mOut[AES_BLOCK_SIZE];
	unsigned char *mOutIndex;
	unsigned char mOutLength;
	bool mIsInit;
	bool mHitEnd;


	IByteReader *mSourceStream;
	AESdecrypt mDecrypt;


	bool DecryptNextBlockAndRefillNext();


};