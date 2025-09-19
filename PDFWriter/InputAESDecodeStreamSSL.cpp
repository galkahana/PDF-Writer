/*
Source File : InputAESDecodeStreamSSL.cpp


Copyright 2025 Gal Kahana PDFWriter

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

#include "InputAESDecodeStreamSSL.h"

#include <algorithm>
#include <string.h>
#include <openssl/evp.h>

using namespace IOBasicTypes;

InputAESDecodeStreamSSL::InputAESDecodeStreamSSL()
{
	mSourceStream = NULL;
	mDecryptCtx = NULL;
	mKey = NULL;
}

InputAESDecodeStreamSSL::InputAESDecodeStreamSSL(
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

	// Create and initialize OpenSSL context
	mDecryptCtx = EVP_CIPHER_CTX_new();
	mIsInit = false; // first read flag. for reading initial IV and first block for decrypting
	mOutLength = AES_BLOCK_SIZE_BYTES; // initially, the length is constant AES block size. when the underlying stream uses padding for the final block, this will be reduced by the padding length
	mOutIndex = mOut + mOutLength; // mOutIndex placed at end of mOut to mark that there's no available decrypted data yet
	mHitEnd = false;
}

InputAESDecodeStreamSSL::~InputAESDecodeStreamSSL(void)
{
	if (mSourceStream)
		delete mSourceStream;

	if (mKey)
		delete[] mKey;

	if (mDecryptCtx)
		EVP_CIPHER_CTX_free(mDecryptCtx);
}

bool InputAESDecodeStreamSSL::NotEnded()
{
	return (mSourceStream && mSourceStream->NotEnded()) || !mHitEnd || ((mOutIndex - mOut) < mOutLength);
}

LongBufferSizeType InputAESDecodeStreamSSL::Read(IOBasicTypes::Byte* inBuffer, LongBufferSizeType inSize)
{
	if (!mSourceStream)
		return 0;

	// first read, this is special case where IV need to be read, followed by an initial block read
	if (!mIsInit) {
		mIsInit = true; // let's get the state change out of the way

		// read iv
		LongBufferSizeType ivRead = mSourceStream->Read(mIV, AES_BLOCK_SIZE_BYTES);
		if (ivRead < AES_BLOCK_SIZE_BYTES)
			return 0;

		// read first block for decryption
		LongBufferSizeType firstBlockLength = mSourceStream->Read(mInNext, AES_BLOCK_SIZE_BYTES);
		if (firstBlockLength < AES_BLOCK_SIZE_BYTES)
			return 0;

		// Initialize OpenSSL decryption context with appropriate cipher based on key length
		const EVP_CIPHER* cipher;
		if (mKeyLength == 16) {
			cipher = EVP_aes_128_cbc();
		} else if (mKeyLength == 32) {
			cipher = EVP_aes_256_cbc();
		} else {
			return 0; // Unsupported key length
		}

		if (EVP_DecryptInit_ex(mDecryptCtx, cipher, NULL, mKey, mIV) != 1)
			return 0;

		// Disable padding as we handle it manually like the original
		EVP_CIPHER_CTX_set_padding(mDecryptCtx, 0);
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

bool InputAESDecodeStreamSSL::DecryptNextBlockAndRefillNext()
{
	// decrypt next block
	memcpy(mIn, mInNext, AES_BLOCK_SIZE_BYTES);

	int outlen;
	if (EVP_DecryptUpdate(mDecryptCtx, mOut, &outlen, mIn, AES_BLOCK_SIZE_BYTES) != 1)
		return false;

	if (outlen != AES_BLOCK_SIZE_BYTES)
		return false; // Should always be AES block size bytes with padding disabled

	// reset available output index
	mOutIndex = mOut;

	// Read next source buffer into next block.
	// this is done now to support padding requirements, so that if we hit the end, we can determine how many bytes are available
	// already in the just decrypted mOut based on read padding size
	LongBufferSizeType totalRead = mSourceStream->Read(mInNext, AES_BLOCK_SIZE_BYTES);
	if (totalRead < AES_BLOCK_SIZE_BYTES) {
		// totalRead should be 0 or 16. So this means we hit the end of the stream, and the just decrypted block is the final block.
		mHitEnd = true;

		// dealing with padding of final block - Determine how much of the decrypted block is actual data by reading the last byte, which should have the padding length.
		size_t paddingLength = std::min<size_t>(mOut[AES_BLOCK_SIZE_BYTES - 1], (size_t)AES_BLOCK_SIZE_BYTES);
		mOutLength = AES_BLOCK_SIZE_BYTES - paddingLength;
	}

	return true;
}