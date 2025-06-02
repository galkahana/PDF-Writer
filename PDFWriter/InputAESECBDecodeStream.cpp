/*
Source File : InputAESECBDecodeStream.h


Copyright 2016 Gal Kahana PDFWriter

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

#include "InputAESECBDecodeStream.h"

#include <algorithm>
#include <string.h>

using namespace IOBasicTypes;


InputAESECBDecodeStream::InputAESECBDecodeStream()
{
	mSourceStream = NULL;
}


InputAESECBDecodeStream::InputAESECBDecodeStream(
	IByteReader* inSourceReader, 
	const ByteList& inKey)
{
	mSourceStream = inSourceReader;

	// convert inEncryptionKey to internal rep and init decrypt. length should be something supported by AES (in bytes, so AES-128 is 16, AES-256 is 32 etc.)
	mKeyLength = inKey.size();
	mKey = new unsigned char[mKeyLength];
	ByteList::const_iterator it = inKey.begin();
	size_t i = 0;
	for (; it != inKey.end(); ++i, ++it)
		mKey[i] = *it;
	mDecrypt.key(mKey, mKeyLength);
	mOutIndex = mOut + AES_BLOCK_SIZE;  // init mOutIndex to point to the end of the output buffer, so we can know we have to fill it up on first read
	mHitEnd = false;
}

InputAESECBDecodeStream::~InputAESECBDecodeStream(void)
{
	if (mSourceStream)
		delete mSourceStream;

	if (mKey)
		delete[] mKey;
}

bool InputAESECBDecodeStream::NotEnded()
{
	return (mSourceStream && mSourceStream->NotEnded()) || !mHitEnd || ((mOutIndex - mOut) < AES_BLOCK_SIZE);
}

LongBufferSizeType InputAESECBDecodeStream::Read(IOBasicTypes::Byte* inBuffer, LongBufferSizeType inSize)
{
	if (!mSourceStream)
		return 0;

	IOBasicTypes::LongBufferSizeType left = inSize;
	IOBasicTypes::LongBufferSizeType remainderRead;

	// fill the buffer by reading decrypted data and refilling it by decrypting following blocks from source stream
	while (left > 0) {
		remainderRead = std::min<size_t>(left,(AES_BLOCK_SIZE - (mOutIndex - mOut)));
		if (remainderRead > 0) {
			// fill block with what's available in the decrypted output buffer
			memcpy(inBuffer + inSize - left, mOutIndex, remainderRead);
			mOutIndex += remainderRead;
			left -= remainderRead;
		}

		if (left > 0) {
			// buffer not full yet, attempt to decrypt from source stream
			if (mHitEnd) {
				// that's true EOF...so finish
				break;
			} else {
				// otherwise, decrypt next block, break on failure
				bool decryptSuccess = DecryptNextBlockAndRefillNext();
				if (!decryptSuccess) {
					break;
				}
			}
		}
	}

	return inSize - left;
}

bool InputAESECBDecodeStream::DecryptNextBlockAndRefillNext()
{
	// read next block from source stream
	LongBufferSizeType totalRead = mSourceStream->Read(mIn, AES_BLOCK_SIZE);
	if (totalRead < AES_BLOCK_SIZE) { 
		// totalRead should be 0 or AES_BLOCK_SIZE. 
		// So this means we hit the end of the stream, and there's no more data to decrypt/read.
		mHitEnd = true; 
		return true;
	}	


	// decrypt next block
	if (mDecrypt.ecb_decrypt(mIn, mOut, AES_BLOCK_SIZE) != EXIT_SUCCESS)
		return false;
	
	// reset available output index
	mOutIndex = mOut;
	
	return true;
}	