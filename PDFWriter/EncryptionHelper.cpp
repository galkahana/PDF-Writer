/*
Source File : EncryptionHelper.cpp


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

#include "EncryptionHelper.h"
#include "OutputStringBufferStream.h"
#include "InputStringStream.h"
#include "OutputStreamTraits.h"
#include "OutputRC4XcodeStream.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"

#include <stdint.h>

using namespace PDFHummus;
using namespace std;

EncryptionHelper::EncryptionHelper(void)
{
	mIsDocumentEncrypted = false;
	mEncryptionPauseLevel = 0;
	mSupportsEncryption = true;
}

EncryptionHelper::~EncryptionHelper(void)
{
}


bool EncryptionHelper::SupportsEncryption() {
	return mSupportsEncryption;
}

bool EncryptionHelper::IsDocumentEncrypted() {
	return mIsDocumentEncrypted;
}

bool EncryptionHelper::IsEncrypting() {
	return IsDocumentEncrypted() && mEncryptionPauseLevel == 0;
}

void EncryptionHelper::PauseEncryption() {
	++mEncryptionPauseLevel;
}

void EncryptionHelper::ReleaseEncryption() {
	--mEncryptionPauseLevel;

}

void EncryptionHelper::OnObjectStart(long long inObjectID, long long inGenerationNumber) {
	if (!IsEncrypting())
		return;

	mXcryption.OnObjectStart(inObjectID, inGenerationNumber);
}
void EncryptionHelper::OnObjectEnd() {
	if (!IsEncrypting())
		return;

	mXcryption.OnObjectEnd();
}

std::string EncryptionHelper::EncryptString(const std::string& inStringToEncrypt) {
	if (!IsEncrypting())
		return inStringToEncrypt;

	OutputStringBufferStream buffer;

	IByteWriterWithPosition* encryptStream = CreateEncryptionWriter(&buffer, mXcryption.GetCurrentObjectKey()); 
	if (encryptStream) {
		InputStringStream inputStream(inStringToEncrypt);
		OutputStreamTraits traits(encryptStream);
		traits.CopyToOutputStream(&inputStream);
		delete encryptStream; // free encryption stream

		return buffer.ToString();
	}
	else
		return inStringToEncrypt;
}

IByteWriterWithPosition* EncryptionHelper::CreateEncryptionStream(IByteWriterWithPosition* inToWrapStream) {
	return  CreateEncryptionWriter(inToWrapStream, mXcryption.GetCurrentObjectKey());
}

IByteWriterWithPosition* EncryptionHelper::CreateEncryptionWriter(IByteWriterWithPosition* inToWrapStream, const ByteList& inEncryptionKey) {
	return new OutputRC4XcodeStream(inToWrapStream, inEncryptionKey,false);
}


static const string scFilter = "Filter";
static const string scStandard = "Standard";
static const string scV = "V";
static const string scLength = "Length";
static const string scR = "R";
static const string scO = "O";
static const string scU = "U";
static const string scP = "P";
static const string scEncryptMetadata = "EncryptMetadata";

EStatusCode EncryptionHelper::WriteEncryptionDictionary(ObjectsContext* inObjectsContext) {
	if (!IsDocumentEncrypted()) // document not encrypted, nothing to write. unexpected
		return eFailure;

	PauseEncryption();
	DictionaryContext* encryptContext = inObjectsContext->StartDictionary();

	// Filter
	encryptContext->WriteKey(scFilter);
	encryptContext->WriteNameValue(scStandard);

	// V
	encryptContext->WriteKey(scV);
	encryptContext->WriteIntegerValue(mV);

	// Length (if not 40. this would allow simple non usage control)
	if (mLength != 5) {
		encryptContext->WriteKey(scLength);
		encryptContext->WriteIntegerValue(mLength * 8);
	}

	// R
	encryptContext->WriteKey(scR);
	encryptContext->WriteIntegerValue(mRevision);

	// O
	encryptContext->WriteKey(scO);
	encryptContext->WriteHexStringValue(mXcryption.ByteListToString(mO));

	// U
	encryptContext->WriteKey(scU);
	encryptContext->WriteHexStringValue(mXcryption.ByteListToString(mU));

	// P
	encryptContext->WriteKey(scP);
	encryptContext->WriteIntegerValue(mP);

	// EncryptMetadata
	encryptContext->WriteKey(scEncryptMetadata);
	encryptContext->WriteBooleanValue(mEncryptMetaData);

	ReleaseEncryption();

	return inObjectsContext->EndDictionary(encryptContext);
}

EStatusCode EncryptionHelper::Setup(
	bool inShouldEncrypt,
	double inPDFLevel,
	const string& inUserPassword,
	const string& inOwnerPassword,
	long long inUserProtectionOptionsFlag,
	bool inEncryptMetadata,
	const string inFileIDPart1
	) {

	if (!inShouldEncrypt) {
		mIsDocumentEncrypted = false;
		mSupportsEncryption = true;
	}

	mIsDocumentEncrypted = false;
	mSupportsEncryption = false;

	do {
		mXcryption.Setup(inPDFLevel);
		if (!mXcryption.CanXCrypt())
			break;

		if (inPDFLevel >= 1.4) {
			mLength = 16;
			mV = 2;
			mRevision = 3;
		}
		else {
			mLength = 5;
			mV = 1;
			mRevision = (inUserProtectionOptionsFlag & 0xF00) ? 3 : 2;
		}


		// compute P out of inUserProtectionOptionsFlag. inUserProtectionOptionsFlag can be a more relaxed number setting as 1s only the enabled access. mP will restrict to PDF Expected bits
		int32_t truncP = int32_t(((inUserProtectionOptionsFlag | 0xFFFFF0C0) & 0xFFFFFFFC));
		mP = truncP;

		ByteList ownerPassword = mXcryption.stringToByteList(inOwnerPassword.size() > 0 ? inOwnerPassword : inUserPassword);
		ByteList userPassword = mXcryption.stringToByteList(inUserPassword);
		mEncryptMetaData = inEncryptMetadata;
		mFileIDPart1 = mXcryption.stringToByteList(inFileIDPart1);

		mO = mXcryption.algorithm3_3(mRevision,mLength,ownerPassword,userPassword);
		if (mRevision == 2)
			mU = mXcryption.algorithm3_4(mLength,userPassword,mO,mP,mFileIDPart1,mEncryptMetaData);
		else
			mU = mXcryption.algorithm3_5(mRevision,mLength,userPassword, mO, mP, mFileIDPart1, mEncryptMetaData);


		mXcryption.SetupInitialEncryptionKey(inUserPassword, mRevision, mLength, mO, mP, mFileIDPart1, mEncryptMetaData);

		mIsDocumentEncrypted = true;
		mSupportsEncryption = true;
	} while (false);

	return eSuccess;
}
