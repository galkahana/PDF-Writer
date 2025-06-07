/*
Source File : InputAESDecodeStream.h


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

#include "InputAESDecodeStream.h"

#include <algorithm>
#include <string.h>

using namespace IOBasicTypes;


InputAESDecodeStream::InputAESDecodeStream()
{
	mSourceStream = NULL;
}


InputAESDecodeStream::InputAESDecodeStream(
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
	mIsInit = false; // first read flag. for reading initial IV and first block for decrypting
	mOutLength = AES_BLOCK_SIZE; // initially, the length is constant AES_BLOCK_SIZE. when the underlying stream uses padding for the final block, this will be reduced by the padding length
	mOutIndex = mOut + mOutLength; // mOutIndex placed at end of mOut to mark that there's no aviailable decrypted data yet
	mHitEnd = false;
}

InputAESDecodeStream::~InputAESDecodeStream(void)
{
	if (mSourceStream)
		delete mSourceStream;

	if (mKey)
		delete[] mKey;
}

bool InputAESDecodeStream::NotEnded()
{
	return (mSourceStream && mSourceStream->NotEnded()) || !mHitEnd || ((mOutIndex - mOut) < mOutLength);
}

LongBufferSizeType InputAESDecodeStream::Read(IOBasicTypes::Byte* inBuffer, LongBufferSizeType inSize)
{
	if (!mSourceStream)
		return 0;

	
	// first read, this is special case where IV need to be read, followed by an initial block read
	if (!mIsInit) {
		mIsInit = true; // let's get the state change out of the way

		// read iv 
		LongBufferSizeType ivRead = mSourceStream->Read(mIV, AES_BLOCK_SIZE);
		if (ivRead < AES_BLOCK_SIZE)
			return 0;

		// read first block for decryption
		LongBufferSizeType firstBlockLength = mSourceStream->Read(mInNext, AES_BLOCK_SIZE);
		if (firstBlockLength < AES_BLOCK_SIZE)
			return 0;
	}


	IOBasicTypes::LongBufferSizeType left = inSize;
	IOBasicTypes::LongBufferSizeType remainderRead;

	// fill the buffer by reading decrypted data and refilling it by decrypting following blocks from source stream
	while (left > 0) {
		remainderRead = std::min<size_t>(left,(mOutLength - (mOutIndex - mOut)));
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

bool InputAESDecodeStream::DecryptNextBlockAndRefillNext()
{
	// decrypt next block
	memcpy(mIn, mInNext, AES_BLOCK_SIZE);
	if (mDecrypt.cbc_decrypt(mIn, mOut, AES_BLOCK_SIZE, mIV) != EXIT_SUCCESS)
		return false;
	
	// reset available output index
	mOutIndex = mOut;
	
	// Read next source buffer into next block.
	// this is done now to support padding requirements, so that if we hit the end, we can determine how many bytes are avialable
	// already in the just decrypted mOut based on read padding size
	LongBufferSizeType totalRead = mSourceStream->Read(mInNext, AES_BLOCK_SIZE);
	if (totalRead < AES_BLOCK_SIZE) { 
		// totalRead should be 0 or AES_BLOCK_SIZE. So this means we hit the end of the stream, and the just decrypted block is the final block.
		mHitEnd = true; 

		// dealing with padding of final block - Determine how much of the decrypted block is actual data by reading the last byte, which should have the padding length.
		size_t paddingLength = std::min<size_t>(mOut[AES_BLOCK_SIZE - 1], AES_BLOCK_SIZE);
		mOutLength = AES_BLOCK_SIZE - paddingLength;
	}	


	return true;
}	