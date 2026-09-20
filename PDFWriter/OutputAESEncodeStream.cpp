/*
Source File : OutputAESEncodeStream.cpp


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

#include "OutputAESEncodeStream.h"
#include "RandomGenerator.h"
#include "aescpp.h"

#include <string.h>

using namespace IOBasicTypes;
using namespace PDFHummus;

OutputAESEncodeStream::OutputAESEncodeStream(void)
{
	mTargetStream = NULL;
	mOwnsStream = false;
	mWroteIV = false;
	mFlushed = true;
}

OutputAESEncodeStream::~OutputAESEncodeStream(void)
{
	Flush();
	if(mEncryptionKey)
		delete[] mEncryptionKey;
	if (mOwnsStream)
		delete mTargetStream;
}

OutputAESEncodeStream::OutputAESEncodeStream(
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
	mEncrypt.key(mEncryptionKey, mEncryptionKeyLength);

	mWroteIV = false;
	mFlushed = false;

}

LongFilePositionType OutputAESEncodeStream::GetCurrentPosition() 
{
	if (mTargetStream)
		return mTargetStream->GetCurrentPosition();
	else
		return 0;
}

EStatusCode OutputAESEncodeStream::EnsureIVWritten()
{
	if (mWroteIV)
		return eSuccess;

	if (RandomGenerator::FillBytes(mIV, AES_BLOCK_SIZE) != eSuccess)
		return eFailure;

	// write IV to output stream
	if (mTargetStream->Write(mIV, AES_BLOCK_SIZE) != AES_BLOCK_SIZE)
		return eFailure;
	mWroteIV = true;
	return eSuccess;
}

LongBufferSizeType OutputAESEncodeStream::Write(const IOBasicTypes::Byte* inBuffer, IOBasicTypes::LongBufferSizeType inSize)
{
	if (!mTargetStream)
		return 0;

	if (EnsureIVWritten() != eSuccess)
		return 0;

	// input and existing buffer sizes smaller than block size, so just copy and return


	IOBasicTypes::LongBufferSizeType left = inSize;

	while (left > 0) {
		// if what's left is not enough to finish block, just copy and stop
		if (left + (mInIndex - mIn) < AES_BLOCK_SIZE) {
			memcpy(mInIndex, inBuffer + inSize - left, left);
			mInIndex += left;
			left = 0;
		}
		else {
			// otherwise, enough to fill block. fill, encode and continue
			IOBasicTypes::LongBufferSizeType remainder = AES_BLOCK_SIZE - (mInIndex - mIn);
			memcpy(mInIndex, inBuffer + inSize - left, remainder);

			// encrypt
			mEncrypt.cbc_encrypt(mIn, mOut, AES_BLOCK_SIZE, mIV);
			if (mTargetStream->Write(mOut, AES_BLOCK_SIZE) != AES_BLOCK_SIZE)
				return 0;
			mInIndex = mIn;
			left -= remainder;
		}
	}

	return inSize;
}

EStatusCode OutputAESEncodeStream::Flush() {
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
		if (mInIndex - mIn == AES_BLOCK_SIZE) {
			mEncrypt.cbc_encrypt(mIn, mOut, AES_BLOCK_SIZE, mIV);
			if (mTargetStream->Write(mOut, AES_BLOCK_SIZE) != AES_BLOCK_SIZE) {
				status = eFailure;
				break;
			}
			mInIndex = mIn;
		}

		// fill the last block with padding bytes. if the last block was full and padding is required still, fill it with the block size (AES_BLOCK_SIZE) as padding bytes
		unsigned char remainder = (unsigned char)(AES_BLOCK_SIZE - (mInIndex - mIn));
		for (size_t i = 0; i < remainder; ++i)
			mInIndex[i] = remainder;
		mEncrypt.cbc_encrypt(mIn, mOut, AES_BLOCK_SIZE, mIV);
		if (mTargetStream->Write(mOut, AES_BLOCK_SIZE) != AES_BLOCK_SIZE)
			status = eFailure;
	} while (false);

	// only cascade into a stream we own - a non-owned target's lifecycle
	// (including when it gets finalized) is managed by whoever gave it to us.
	if (mOwnsStream && mTargetStream && mTargetStream->Flush() != eSuccess)
		status = eFailure;

	return status;
}