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
#include "RandomGenerator.h"

#include <string.h>
#include <openssl/evp.h>

using namespace IOBasicTypes;
using namespace PDFHummus;

OutputAESEncodeStreamSSL::OutputAESEncodeStreamSSL(void)
{
	mTargetStream = NULL;
	mOwnsStream = false;
	mWroteIV = false;
	mFlushed = true;
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
	mFlushed = false;
}

LongFilePositionType OutputAESEncodeStreamSSL::GetCurrentPosition()
{
	if (mTargetStream)
		return mTargetStream->GetCurrentPosition();
	else
		return 0;
}

EStatusCode OutputAESEncodeStreamSSL::EnsureIVWritten()
{
	if (mWroteIV)
		return eSuccess;

	if (RandomGenerator::FillBytes(mIV, AES_BLOCK_SIZE_BYTES) != eSuccess)
		return eFailure;

	// Initialize OpenSSL encryption context with appropriate cipher based on key length
	const EVP_CIPHER* cipher;
	if (mEncryptionKeyLength == 16) {
		cipher = EVP_aes_128_cbc();
	} else if (mEncryptionKeyLength == 32) {
		cipher = EVP_aes_256_cbc();
	} else {
		return eFailure; // Unsupported key length
	}

	if (EVP_EncryptInit_ex(mEncryptCtx, cipher, NULL, mEncryptionKey, mIV) != 1)
		return eFailure;

	// Disable padding as we handle it manually like the original
	EVP_CIPHER_CTX_set_padding(mEncryptCtx, 0);

	// only commit the IV to output once key validation and cipher init succeeded
	if (mTargetStream->Write(mIV, AES_BLOCK_SIZE_BYTES) != AES_BLOCK_SIZE_BYTES)
		return eFailure;

	mWroteIV = true;
	return eSuccess;
}

LongBufferSizeType OutputAESEncodeStreamSSL::Write(const IOBasicTypes::Byte* inBuffer, IOBasicTypes::LongBufferSizeType inSize)
{
	if (!mTargetStream)
		return 0;

	if (EnsureIVWritten() != eSuccess)
		return 0;

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

			if (mTargetStream->Write(mOut, AES_BLOCK_SIZE_BYTES) != AES_BLOCK_SIZE_BYTES)
				return 0;
			mInIndex = mIn;
			left -= remainder;
		}
	}

	return inSize;
}

EStatusCode OutputAESEncodeStreamSSL::Flush() {
	EStatusCode status = eSuccess;

	do {
		if (mFlushed)
			break;
		mFlushed = true;

		if (!mTargetStream)
			break;

		if (EnsureIVWritten() != eSuccess) {
			status = eFailure;
			break;
		}

		// if there's a full buffer waiting, write it now.
		if (mInIndex - mIn == AES_BLOCK_SIZE_BYTES) {
			int outlen;
			if (EVP_EncryptUpdate(mEncryptCtx, mOut, &outlen, mIn, AES_BLOCK_SIZE_BYTES) != 1 || outlen != AES_BLOCK_SIZE_BYTES) {
				status = eFailure;
				break;
			}
			if (mTargetStream->Write(mOut, AES_BLOCK_SIZE_BYTES) != AES_BLOCK_SIZE_BYTES) {
				status = eFailure;
				break;
			}
			mInIndex = mIn;
		}

		// fill the last block with padding bytes. if the last block was full and padding is required still, fill it with the block size as padding bytes
		unsigned char remainder = (unsigned char)(AES_BLOCK_SIZE_BYTES - (mInIndex - mIn));
		for (size_t i = 0; i < remainder; ++i)
			mInIndex[i] = remainder;

		int outlen;
		if (EVP_EncryptUpdate(mEncryptCtx, mOut, &outlen, mIn, AES_BLOCK_SIZE_BYTES) != 1 || outlen != AES_BLOCK_SIZE_BYTES) {
			status = eFailure;
			break;
		}
		if (mTargetStream->Write(mOut, AES_BLOCK_SIZE_BYTES) != AES_BLOCK_SIZE_BYTES)
			status = eFailure;
	} while (false);

	// only cascade into a stream we own - a non-owned target's lifecycle
	// (including when it gets finalized) is managed by whoever gave it to us.
	if (mOwnsStream && mTargetStream && mTargetStream->Flush() != eSuccess)
		status = eFailure;

	return status;
}