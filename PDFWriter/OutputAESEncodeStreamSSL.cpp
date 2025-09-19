/*
Source File : OutputAESEncodeStreamSSL.cpp


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

#include "OutputAESEncodeStreamSSL.h"
#include "MD5Generator.h"
#include "PDFDate.h"

#include <string.h>
#include <openssl/evp.h>

using namespace IOBasicTypes;

OutputAESEncodeStreamSSL::OutputAESEncodeStreamSSL(void)
{
	mTargetStream = NULL;
	mOwnsStream = false;
	mWroteIV = false;
	mEncryptCtx = NULL;
	mEncryptionKey = NULL;
}

OutputAESEncodeStreamSSL::~OutputAESEncodeStreamSSL(void)
{
	Flush();
	if(mEncryptionKey)
		delete[] mEncryptionKey;
	if (mOwnsStream)
		delete mTargetStream;
	if (mEncryptCtx)
		EVP_CIPHER_CTX_free(mEncryptCtx);
}

OutputAESEncodeStreamSSL::OutputAESEncodeStreamSSL(
	IByteWriterWithPosition* inTargetStream,
	const ByteList& inEncryptionKey,
	bool inOwnsStream)
{
	mTargetStream = inTargetStream;
	mOwnsStream = inOwnsStream;

	if (!mTargetStream)
		return;

	mInIndex = mIn;

	// convert inEncryptionKey to internal rep and init encrypt. length should be something supported by AES (in bytes, so AES-128 is 16, AES-256 is 32 etc.)
	mEncryptionKey = new unsigned char[inEncryptionKey.size()];
	mEncryptionKeyLength = inEncryptionKey.size();
	ByteList::const_iterator it = inEncryptionKey.begin();
	size_t i = 0;
	for (; it != inEncryptionKey.end(); ++i, ++it)
		mEncryptionKey[i] = *it;

	// Create OpenSSL context
	mEncryptCtx = EVP_CIPHER_CTX_new();
	mWroteIV = false;
}

LongFilePositionType OutputAESEncodeStreamSSL::GetCurrentPosition()
{
	if (mTargetStream)
		return mTargetStream->GetCurrentPosition();
	else
		return 0;
}

LongBufferSizeType OutputAESEncodeStreamSSL::Write(const IOBasicTypes::Byte* inBuffer, IOBasicTypes::LongBufferSizeType inSize)
{
	if (!mTargetStream)
		return 0;

	// write IV if didn't write yet
	if (!mWroteIV) {
		// random IV using MD5 of current time
		MD5Generator md5;
		// encode current time
		PDFDate currentTime;
		currentTime.SetToCurrentTime();
		md5.Accumulate(currentTime.ToString());
		memcpy(mIV, (const unsigned char*)md5.ToStringAsString().c_str(), AES_BLOCK_SIZE_BYTES); // md5 should give us the desired AES block size bytes
		// write IV to output stream
		mTargetStream->Write(mIV, AES_BLOCK_SIZE_BYTES);

		// Initialize OpenSSL encryption context with appropriate cipher based on key length
		const EVP_CIPHER* cipher;
		if (mEncryptionKeyLength == 16) {
			cipher = EVP_aes_128_cbc();
		} else if (mEncryptionKeyLength == 32) {
			cipher = EVP_aes_256_cbc();
		} else {
			return 0; // Unsupported key length
		}

		if (EVP_EncryptInit_ex(mEncryptCtx, cipher, NULL, mEncryptionKey, mIV) != 1)
			return 0;

		// Disable padding as we handle it manually like the original
		EVP_CIPHER_CTX_set_padding(mEncryptCtx, 0);

		mWroteIV = true;
	}

	IOBasicTypes::LongBufferSizeType left = inSize;

	while (left > 0) {
		// if what's left is not enough to finish block, just copy and stop
		if (left + (mInIndex - mIn) < AES_BLOCK_SIZE_BYTES) {
			memcpy(mInIndex, inBuffer + inSize - left, left);
			mInIndex += left;
			left = 0;
		}
		else {
			// otherwise, enough to fill block. fill, encode and continue
			IOBasicTypes::LongBufferSizeType remainder = AES_BLOCK_SIZE_BYTES - (mInIndex - mIn);
			memcpy(mInIndex, inBuffer + inSize - left, remainder);

			// encrypt
			int outlen;
			if (EVP_EncryptUpdate(mEncryptCtx, mOut, &outlen, mIn, AES_BLOCK_SIZE_BYTES) != 1)
				return 0;

			if (outlen != AES_BLOCK_SIZE_BYTES)
				return 0; // Should always be AES block size bytes with padding disabled

			mTargetStream->Write(mOut, AES_BLOCK_SIZE_BYTES);
			mInIndex = mIn;
			left -= remainder;
		}
	}

	return inSize;
}

void OutputAESEncodeStreamSSL::Flush() {
	if (!mTargetStream)
		return;

	// if there's a full buffer waiting, write it now.
	if (mInIndex - mIn == AES_BLOCK_SIZE_BYTES) {
		int outlen;
		if (EVP_EncryptUpdate(mEncryptCtx, mOut, &outlen, mIn, AES_BLOCK_SIZE_BYTES) == 1 && outlen == AES_BLOCK_SIZE_BYTES) {
			mTargetStream->Write(mOut, AES_BLOCK_SIZE_BYTES);
		}
		mInIndex = mIn;
	}

	// fill the last block with padding bytes. if the last block was full and padding is required still, fill it with the block size as padding bytes
	unsigned char remainder = (unsigned char)(AES_BLOCK_SIZE_BYTES - (mInIndex - mIn));
	for (size_t i = 0; i < remainder; ++i)
		mInIndex[i] = remainder;

	int outlen;
	if (EVP_EncryptUpdate(mEncryptCtx, mOut, &outlen, mIn, AES_BLOCK_SIZE_BYTES) == 1 && outlen == AES_BLOCK_SIZE_BYTES) {
		mTargetStream->Write(mOut, AES_BLOCK_SIZE_BYTES);
	}
}