/*
Source File : XCryptor.cpp


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
#include "XCryptor.h"
#include "XCryptionCommon.h"
#include "IOBasicTypes.h"

using namespace IOBasicTypes;

XCryptor::XCryptor(bool inUsingAES, const ByteList& inFileEncryptionKey)
{
	mUsingAES = inUsingAES;
	mFileEncryptionKey = inFileEncryptionKey;
}

XCryptor::~XCryptor(void)
{
}

bool XCryptor::GetIsUsingAES() {
	return mUsingAES;
}

const ByteList& XCryptor::GetFileEncryptionKey()
{
	return mFileEncryptionKey;
}

const ByteList& XCryptor::OnObjectStart(long long inObjectID, long long inGenerationNumber) {
    XCryptionCommon xcryption;

	mEncryptionKeysStack.push_back(
		xcryption.RetrieveObjectEncryptionKey(
			mFileEncryptionKey,
			mUsingAES,
			(ObjectIDType)inObjectID, 
			(unsigned long)inGenerationNumber
		)
	);

	return mEncryptionKeysStack.back();
}

void XCryptor::OnObjectEnd() {
	mEncryptionKeysStack.pop_back();
}

const ByteList scEmptyByteList;

const ByteList& XCryptor::GetCurrentObjectKey() {
	return  mEncryptionKeysStack.size() > 0 ? mEncryptionKeysStack.back() : scEmptyByteList;
}

