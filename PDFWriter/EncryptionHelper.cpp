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
#include "OutputAESEncodeStream.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "DecryptionHelper.h"
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFLiteralString.h"
#include "PDFInteger.h"
#include "PDFBoolean.h"
#include "XCryptionCommon.h"
#include "XCryptionCommon2_0.h"
#include "Trace.h"
#include <stdint.h>

using namespace PDFHummus;
using namespace std;

EncryptionHelper::EncryptionHelper(void)
{
	mIsDocumentEncrypted = false;
	mEncryptionPauseLevel = 0;
	mSupportsEncryption = true;
	mXcryptStreams = NULL;
	mXcryptStrings = NULL;

	mV = 0;
	mLength = 0;
	mRevision = 0;
	mP = 0;
	mEncryptMetaData = false;
}

EncryptionHelper::~EncryptionHelper(void)
{
	Release();
}

void EncryptionHelper::Release() {
	StringToXCryptorMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it)
		delete it->second;
	mXcrypts.clear();
}

static const string scStdCF = "StdCF";
static const ByteList scEmptyByteList;


void EncryptionHelper::Setup(
	bool inShouldEncrypt,
	double inPDFLevel,
	const string& inUserPassword,
	const string& inOwnerPassword,
	long long inUserProtectionOptionsFlag,
	bool inEncryptMetadata,
	const string inFileIDPart1
	) {

	if (!inShouldEncrypt) {
		SetupNoEncryption();
		return;
	}

	mIsDocumentEncrypted = false;
	mSupportsEncryption = false;

	// Determine mV (encryption algorithm version),  mRevision (standard security handler revision), and mLength (encryption key length) based on PDF level, using the strongest encryption that the PDF version allows
	if (inPDFLevel >= 1.4) {
		if (inPDFLevel >= 2.0) {
#ifdef PDFHUMMUS_NO_OPENSSL
			// PDF 2.0 is not supported without OpenSSL
			TRACE_LOG("EncryptionHelper::Setup - PDF 2.0 encryption is not supported without open ssl. either install OpenSSL or use a lower PDF level. aborting.");
			return;
#else		
			mLength = 32;
			mV = 5;
			mRevision = 6;
#endif			
		}
		else 
		{
			mLength = 16;
			if (inPDFLevel >= 1.6) {
				mV = 4;
				mRevision = 4;
			}
			else 
			{
				mV = 2;
				mRevision = 3;
			}
		}
	}
	else {
		mLength = 5;
		mV = 1;
		mRevision = (inUserProtectionOptionsFlag & 0xF00) ? 3 : 2;
	}

	// compute P out of inUserProtectionOptionsFlag. inUserProtectionOptionsFlag can be a more relaxed number setting as 1s only the enabled access. mP will restrict to PDF Expected bits
	int32_t truncP = int32_t(((inUserProtectionOptionsFlag | 0xFFFFF0C0) & 0xFFFFFFFC));
	mP = truncP;

	ByteList ownerPassword = stringToByteList(inOwnerPassword.size() > 0 ? inOwnerPassword : inUserPassword);
	ByteList userPassword = stringToByteList(inUserPassword);
	mEncryptMetaData = inEncryptMetadata;
	mFileIDPart1 = stringToByteList(inFileIDPart1);

	ByteList fileEncryptionKey;
	if(mV >= 5) {
		// PDF 2.0 algos
#ifndef PDFHUMMUS_NO_OPENSSL
		XCryptionCommon2_0 xcryptionCommon2_0;
		fileEncryptionKey = xcryptionCommon2_0.GenerateFileEncryptionKey();
		ByteListPair uAndUE = xcryptionCommon2_0.CreateUandUEValues(userPassword, fileEncryptionKey);
		mU = uAndUE.first;
		mUE = uAndUE.second;
		ByteListPair oAndOE = xcryptionCommon2_0.CreateOandOEValues(ownerPassword, fileEncryptionKey, mU);
		mO = oAndOE.first;
		mOE = oAndOE.second;
		mPerms = xcryptionCommon2_0.CreatePerms(fileEncryptionKey, mP, mEncryptMetaData);
#endif		
	} else {
		// Pre PDF 2.0 algos
		XCryptionCommon xcryptionCommon;
		mO = xcryptionCommon.CreateOValue(mRevision,mLength,ownerPassword,userPassword);
		fileEncryptionKey = xcryptionCommon.RetrieveFileEncryptionKey(
			userPassword,
			mRevision,
			mLength,
			mO,
			mP,
			mFileIDPart1,
			mEncryptMetaData);
		mU = xcryptionCommon.CreateUValue(
			fileEncryptionKey,
			mRevision,
			mFileIDPart1);	
		// mOE and mUE do not exist prior to PDF 2.0, provide them with empty values
		mOE = scEmptyByteList;
		mUE = scEmptyByteList;
	}

	EXCryptorAlgo xcryptorAlgo = (mV < 4) ? eRC4 : ((mV == 4) ? eAESV2 : eAESV3);
	XCryptor* defaultEncryption = new XCryptor(xcryptorAlgo, fileEncryptionKey);
	mXcrypts.insert(StringToXCryptorMap::value_type(scStdCF, defaultEncryption));
	mXcryptStreams = defaultEncryption;
	mXcryptStrings = defaultEncryption;

	mIsDocumentEncrypted = true;
	mSupportsEncryption = true;
}

void EncryptionHelper::SetupNoEncryption() 
{
	mIsDocumentEncrypted = false;
	mSupportsEncryption = true;
}

void EncryptionHelper::Setup(const DecryptionHelper& inDecryptionSource) 
{
	// This would be a setup for modified PDF, where the encryption setup is borrowing from the parser descryption setup
	if (!inDecryptionSource.IsEncrypted() || !inDecryptionSource.CanDecryptDocument()) {
		SetupNoEncryption();
		return;
	}

	mIsDocumentEncrypted = true;
	mSupportsEncryption = true;

	mLength = inDecryptionSource.GetLength();
	mV = inDecryptionSource.GetV();
	mRevision = inDecryptionSource.GetRevision();
	mP = inDecryptionSource.GetP();
	mEncryptMetaData = inDecryptionSource.GetEncryptMetaData();
	mFileIDPart1 = inDecryptionSource.GetFileIDPart1();
	mO = inDecryptionSource.GetO();
	mU = inDecryptionSource.GetU();
	mOE = inDecryptionSource.GetOE();
	mUE = inDecryptionSource.GetUE();
	mPerms = inDecryptionSource.GetPerms();

	// initialize xcryptors
	mXcryptStreams = NULL;
	// xcrypt to use for strings
	mXcryptStrings = NULL;
	StringToXCryptorMap::const_iterator it = inDecryptionSource.GetXcrypts().begin();
	StringToXCryptorMap::const_iterator itEnd = inDecryptionSource.GetXcrypts().end();
	for (; it != itEnd; ++it) {
		// creating parallel copies for 
		XCryptor* xCryption = new XCryptor(it->second->GetExcryptorAlgo(), it->second->GetFileEncryptionKey());
		mXcrypts.insert(StringToXCryptorMap::value_type(it->first, xCryption));

		// see if it fits any of the global xcryptors
		if (it->second == inDecryptionSource.GetStreamXcrypt())
			mXcryptStreams = xCryption;
		if (it->second == inDecryptionSource.GetStringXcrypt())
			mXcryptStrings = xCryption;

		// it is expected that one of those xcryptor has "StdCF" as its key
	}
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

	StringToXCryptorMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it) {
		it->second->OnObjectStart(inObjectID, inGenerationNumber);
	}
}
void EncryptionHelper::OnObjectEnd() {
	if (!IsEncrypting())
		return;

	StringToXCryptorMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it) {
		it->second->OnObjectEnd();
	}
}

std::string EncryptionHelper::EncryptString(const std::string& inStringToEncrypt) {
	if (!IsEncrypting() || !mXcryptStrings)
		return inStringToEncrypt;

	OutputStringBufferStream buffer;

	IByteWriterWithPosition* encryptStream = CreateEncryptionWriter(&buffer, mXcryptStrings->GetCurrentObjectKey(), mXcryptStrings->GetIsUsingAES());
	if (encryptStream) {
		InputStringStream inputStream(inStringToEncrypt);
		OutputStreamTraits traits(encryptStream);
		traits.CopyToOutputStream(&inputStream);
		delete encryptStream; // free encryption stream (sometimes it will also mean flushing the output stream)

		return buffer.ToString();
	}
	else
		return inStringToEncrypt;
}

IByteWriterWithPosition* EncryptionHelper::CreateEncryptionStream(IByteWriterWithPosition* inToWrapStream) {
	return  CreateEncryptionWriter(inToWrapStream, mXcryptStreams->GetCurrentObjectKey(),mXcryptStreams->GetIsUsingAES());
}

IByteWriterWithPosition* EncryptionHelper::CreateEncryptionWriter(IByteWriterWithPosition* inToWrapStream, const ByteList& inEncryptionKey, bool inIsUsingAES) {
	if (inIsUsingAES) {
		return new OutputAESEncodeStream(inToWrapStream, inEncryptionKey, false);
	}
	else {
		return new OutputRC4XcodeStream(inToWrapStream, inEncryptionKey, false);
	}
}


static const string scFilter = "Filter";
static const string scStandard = "Standard";
static const string scV = "V";
static const string scLength = "Length";
static const string scR = "R";
static const string scO = "O";
static const string scU = "U";
static const string scP = "P";
static const string scOE = "OE";
static const string scUE = "UE";
static const string scEncryptMetadata = "EncryptMetadata";
static const string scPerms = "Perms";

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
	encryptContext->WriteHexStringValue(ByteListToString(mO));

	// U
	encryptContext->WriteKey(scU);
	encryptContext->WriteHexStringValue(ByteListToString(mU));


	// P
	encryptContext->WriteKey(scP);
	encryptContext->WriteIntegerValue(mP);

	// PDF 2.0/v5 also writes OE, UE and Perms
	if (mV >= 5) {
		// OE
		encryptContext->WriteKey(scOE);
		encryptContext->WriteHexStringValue(ByteListToString(mOE));

		// UE
		encryptContext->WriteKey(scUE);
		encryptContext->WriteHexStringValue(ByteListToString(mUE));

		// Perms
		encryptContext->WriteKey(scPerms);
		encryptContext->WriteHexStringValue(ByteListToString(mPerms));
	}

	// EncryptMetadata
	encryptContext->WriteKey(scEncryptMetadata);
	encryptContext->WriteBooleanValue(mEncryptMetaData);

	// Now. if using V4 or V5, define crypt filters
	if (mV == 4 || mV == 5) {
		encryptContext->WriteKey("CF");
		DictionaryContext* cf = inObjectsContext->StartDictionary();
		cf->WriteKey(scStdCF);

		DictionaryContext* stdCf = inObjectsContext->StartDictionary();
		stdCf->WriteKey("Type");
		stdCf->WriteNameValue("CryptFilter");


		stdCf->WriteKey("CFM");
		// V4 uses AES with 128 bit key, V5 uses AES with 256 bit key
		stdCf->WriteNameValue(mV == 4 ? "AESV2": "AESV3");

		stdCf->WriteKey("AuthEvent");
		stdCf->WriteNameValue("DocOpen");

		stdCf->WriteKey("Length");
		stdCf->WriteIntegerValue(mLength * 8);

		inObjectsContext->EndDictionary(stdCf);
		inObjectsContext->EndDictionary(cf);

		encryptContext->WriteKey("StmF");
		encryptContext->WriteNameValue(scStdCF);

		encryptContext->WriteKey("StrF");
		encryptContext->WriteNameValue(scStdCF);

	}

	ReleaseEncryption();

	return inObjectsContext->EndDictionary(encryptContext);
}

EStatusCode EncryptionHelper::WriteState(ObjectsContext* inStateWriter, ObjectIDType inObjectID)
{
	EStatusCode status = inStateWriter->StartNewIndirectObject(inObjectID);
	if(status != eSuccess)
		return status;
	DictionaryContext* encryptionObject = inStateWriter->StartDictionary();

	encryptionObject->WriteKey("Type");
	encryptionObject->WriteNameValue("EncryptionHelper");

	encryptionObject->WriteKey("mIsDocumentEncrypted");
	encryptionObject->WriteBooleanValue(mIsDocumentEncrypted);

	encryptionObject->WriteKey("mSupportsEncryption");
	encryptionObject->WriteBooleanValue(mSupportsEncryption);



	encryptionObject->WriteKey("mLength");
	encryptionObject->WriteIntegerValue(mLength);

	encryptionObject->WriteKey("mV");
	encryptionObject->WriteIntegerValue(mV);

	encryptionObject->WriteKey("mRevision");
	encryptionObject->WriteIntegerValue(mRevision);

	encryptionObject->WriteKey("mP");
	encryptionObject->WriteIntegerValue(mP);

	encryptionObject->WriteKey("mEncryptMetaData");
	encryptionObject->WriteBooleanValue(mEncryptMetaData);

	encryptionObject->WriteKey("mFileIDPart1");
	encryptionObject->WriteLiteralStringValue(ByteListToString(mFileIDPart1));

	encryptionObject->WriteKey("mO");
	encryptionObject->WriteLiteralStringValue(ByteListToString(mO));

	encryptionObject->WriteKey("mU");
	encryptionObject->WriteLiteralStringValue(ByteListToString(mU));

	encryptionObject->WriteKey("mOE");
	encryptionObject->WriteLiteralStringValue(ByteListToString(mOE));

	encryptionObject->WriteKey("mUE");
	encryptionObject->WriteLiteralStringValue(ByteListToString(mUE));	

	encryptionObject->WriteKey("mPerms");
	encryptionObject->WriteLiteralStringValue(ByteListToString(mPerms));

	StringToXCryptorMap::const_iterator it = mXcrypts.find(scStdCF);
	XCryptor* defaultEncryption = (it == mXcrypts.end()) ? NULL: it->second;

	if(defaultEncryption)
	{
		encryptionObject->WriteKey("DefaultEncryption");
		DictionaryContext* defaultEncryptionObject = inStateWriter->StartDictionary();
		defaultEncryptionObject->WriteKey("XCryptorAlgo");
		defaultEncryptionObject->WriteIntegerValue(defaultEncryption->GetExcryptorAlgo());
		defaultEncryptionObject->WriteKey("FileEncryptionKey");
		defaultEncryptionObject->WriteLiteralStringValue(ByteListToString(defaultEncryption->GetFileEncryptionKey()));
		inStateWriter->EndDictionary(defaultEncryptionObject);
	}

	inStateWriter->EndDictionary(encryptionObject);
	inStateWriter->EndIndirectObject();

	return status;
}

PDFHummus::EStatusCode EncryptionHelper::ReadState(PDFParser* inStateReader, ObjectIDType inObjectID)
{
	PDFObjectCastPtr<PDFDictionary> encryptionObjectState(inStateReader->ParseNewObject(inObjectID));

	PDFObjectCastPtr<PDFBoolean> isDocumentEncrypted = encryptionObjectState->QueryDirectObject("mIsDocumentEncrypted");
	mIsDocumentEncrypted = isDocumentEncrypted->GetValue();

	PDFObjectCastPtr<PDFBoolean> supportsEncryption = encryptionObjectState->QueryDirectObject("mSupportsEncryption");
	mSupportsEncryption = supportsEncryption->GetValue();


	PDFObjectCastPtr<PDFInteger> length = encryptionObjectState->QueryDirectObject("mLength");
	mLength = (unsigned int)length->GetValue();

	PDFObjectCastPtr<PDFInteger> v = encryptionObjectState->QueryDirectObject("mV");
	mV = (unsigned int)v->GetValue();

	PDFObjectCastPtr<PDFInteger> revision = encryptionObjectState->QueryDirectObject("mRevision");
	mRevision = (unsigned int)revision->GetValue();

	PDFObjectCastPtr<PDFInteger> p = encryptionObjectState->QueryDirectObject("mP");
	mP = p->GetValue();

	PDFObjectCastPtr<PDFBoolean> encryptMetaData = encryptionObjectState->QueryDirectObject("mEncryptMetaData");
	mEncryptMetaData = encryptMetaData->GetValue();

	PDFObjectCastPtr<PDFLiteralString> fileIDPart1 = encryptionObjectState->QueryDirectObject("mFileIDPart1");
	mFileIDPart1 = stringToByteList(fileIDPart1->GetValue());

	PDFObjectCastPtr<PDFLiteralString> o = encryptionObjectState->QueryDirectObject("mO");
	mO = stringToByteList(o->GetValue());

	PDFObjectCastPtr<PDFLiteralString> u = encryptionObjectState->QueryDirectObject("mU");
	mU = stringToByteList(u->GetValue());


	PDFObjectCastPtr<PDFLiteralString> oE = encryptionObjectState->QueryDirectObject("mOE");
	mOE = stringToByteList(oE->GetValue());

	PDFObjectCastPtr<PDFLiteralString> uE = encryptionObjectState->QueryDirectObject("mUE");
	mUE = stringToByteList(uE->GetValue());	

	PDFObjectCastPtr<PDFLiteralString> perms = encryptionObjectState->QueryDirectObject("mPerms");
	mPerms = stringToByteList(perms->GetValue());

	// setup default encryption
	PDFObjectCastPtr<PDFDictionary> defaultEncryptionObject = encryptionObjectState->QueryDirectObject("DefaultEncryption");
	if(!!defaultEncryptionObject) {
		PDFObjectCastPtr<PDFInteger> xCryptorAlgoObject = defaultEncryptionObject->QueryDirectObject("XCryptorAlgo");
		if(!xCryptorAlgoObject)
			return eFailure;

		EXCryptorAlgo xCryptorAlgo = (EXCryptorAlgo)xCryptorAlgoObject->GetValue();

		PDFObjectCastPtr<PDFLiteralString> fileEncryptionKeyObject = defaultEncryptionObject->QueryDirectObject("FileEncryptionKey");
		if(!fileEncryptionKeyObject)
			return eFailure;

		ByteList fileEncryptionKey = stringToByteList(fileEncryptionKeyObject->GetValue());

		XCryptor* defaultEncryption = new XCryptor(xCryptorAlgo, fileEncryptionKey);
		mXcrypts.insert(StringToXCryptorMap::value_type(scStdCF, defaultEncryption));
		mXcryptStreams = defaultEncryption;
		mXcryptStrings = defaultEncryption;
	}

	return eSuccess;
}
