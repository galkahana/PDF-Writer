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
#include "XCryptionCommon2_0.h"
#include "IOBasicTypes.h"

using namespace IOBasicTypes;

static const ByteList scEmptyByteList;

XCryptor::XCryptor(EXCryptorAlgo inExcryptorAlgo, const ByteList& inFileEncryptionKey)
{
	mExcryptorAlgo = inExcryptorAlgo;
	mFileEncryptionKey = inFileEncryptionKey;
}

XCryptor::~XCryptor(void)
{
}

EXCryptorAlgo XCryptor::GetExcryptorAlgo() const {
	return mExcryptorAlgo;
}

bool XCryptor::GetIsUsingAES() const {
	return mExcryptorAlgo == eAESV2 || mExcryptorAlgo == eAESV3;
}

bool XCryptor::GetIsUsing2_0() const {
	return mExcryptorAlgo == eAESV3;
}

const ByteList& XCryptor::GetFileEncryptionKey() const
{
	return mFileEncryptionKey;
}

const ByteList& XCryptor::OnObjectStart(long long inObjectID, long long inGenerationNumber) {
	if (GetIsUsing2_0()) {
		// PDF2.0 always uses the file encyption key. no need for considering object keys.
		return mFileEncryptionKey;
	}

    XCryptionCommon xcryption;

	mEncryptionKeysStack.push_back(
		xcryption.RetrieveObjectEncryptionKey(
			mFileEncryptionKey,
			GetIsUsingAES(),
			(ObjectIDType)inObjectID, 
			(unsigned long)inGenerationNumber
		)
	);

	return mEncryptionKeysStack.back();
}

void XCryptor::OnObjectEnd() {
	if(GetIsUsing2_0())
		return;

	mEncryptionKeysStack.pop_back();
}

const ByteList& XCryptor::GetCurrentObjectKey() const {
	if(GetIsUsing2_0()) {
		return mFileEncryptionKey;
	}

	return  mEncryptionKeysStack.size() > 0 ? mEncryptionKeysStack.back() : scEmptyByteList;
}

