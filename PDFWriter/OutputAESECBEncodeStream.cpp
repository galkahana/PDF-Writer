/*
Source File : OutputAESECBEncodeStream.cpp


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

#include "OutputAESECBEncodeStream.h"

#include <string.h>

using namespace IOBasicTypes;


OutputAESECBEncodeStream::OutputAESECBEncodeStream(void)
{
	mTargetStream = NULL;
	mOwnsStream = false;
}

OutputAESECBEncodeStream::~OutputAESECBEncodeStream(void)
{
	Flush();
	if(mEncryptionKey)
		delete[] mEncryptionKey;
	if (mOwnsStream)
		delete mTargetStream;
}

OutputAESECBEncodeStream::OutputAESECBEncodeStream(
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
}

LongFilePositionType OutputAESECBEncodeStream::GetCurrentPosition() 
{
	if (mTargetStream)
		return mTargetStream->GetCurrentPosition();
	else
		return 0;
}

LongBufferSizeType OutputAESECBEncodeStream::Write(const IOBasicTypes::Byte* inBuffer, IOBasicTypes::LongBufferSizeType inSize) 
{
	if (!mTargetStream)
		return 0;

	// input and existing buffer sizes smaller than block size, so just copy and return
	IOBasicTypes::LongBufferSizeType left = inSize;

	while (left > 0) {
		// if what's left is not enough to finish block, just copy and stop
		if (left + (mInIndex - mIn) < AES_BLOCK_SIZE) {
			memcpy(mInIndex, inBuffer + inSize - left, left);
			mInIndex += left;
			left = 0;
		} else {
			// otherwise, enough to fill block. fill, encode and continue
			IOBasicTypes::LongBufferSizeType remainder = AES_BLOCK_SIZE - (mInIndex - mIn);
			memcpy(mInIndex, inBuffer + inSize - left, remainder);

			// encrypt
			mEncrypt.ecb_encrypt(mIn, mOut, AES_BLOCK_SIZE);
			mTargetStream->Write(mOut, AES_BLOCK_SIZE);
			mInIndex = mIn;
			left -= remainder;
		}
	}

	return inSize;
}

void OutputAESECBEncodeStream::Flush() {
	// if there's a full buffer waiting, write it now. (there can be either full buffer, or 0 bytes. no partial buffer and padding support is required here)
	if (mInIndex - mIn == AES_BLOCK_SIZE) {
		mEncrypt.ecb_encrypt(mIn, mOut, AES_BLOCK_SIZE);
		mTargetStream->Write(mOut, AES_BLOCK_SIZE);
		mInIndex = mIn;
	}
}