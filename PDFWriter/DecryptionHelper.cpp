/*
Source File : DecryptionHelper.cpp


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
#include "DecryptionHelper.h"
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFArray.h"
#include "PDFObject.h"
#include "PDFName.h"
#include "PDFBoolean.h"
#include "PDFStreamInput.h"
#include "InputStringStream.h"
#include "OutputStringBufferStream.h"
#include "OutputStreamTraits.h"
#include "ParsedPrimitiveHelper.h"
#include "RefCountPtr.h"
#include "OutputStringBufferStream.h"
#include "InputRC4XcodeStream.h"
#include "Trace.h"

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

DecryptionHelper::DecryptionHelper(void)
{
	Reset();
}

void DecryptionHelper::Release() {
	StringToXCryptionCommonMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it)
		delete it->second;
	mXcrypts.clear();
}

DecryptionHelper::~DecryptionHelper(void)
{
	Release();
}

void DecryptionHelper::Reset() {
	mSupportsDecryption = false;
	mFailedPasswordVerification = false;
	mDidSucceedOwnerPasswordVerification = false;
	mIsEncrypted = false;
	mXcryptStreams = NULL;
	mXcryptStrings = NULL;
	mXcryptAuthentication = NULL;
	Release();
}

unsigned int ComputeLength(PDFObject* inLengthObject) {
	ParsedPrimitiveHelper lengthHelper(inLengthObject);
	unsigned int value = lengthHelper.IsNumber() ? (unsigned int)lengthHelper.GetAsInteger() : 40;
	return value < 40 ? value : (value / 8); // this small check here is based on some errors i saw, where the length was given in bytes instead of bits
}

XCryptionCommon* GetFilterForName(const StringToXCryptionCommonMap& inXcryptions, const string& inName) {
	StringToXCryptionCommonMap::const_iterator it = inXcryptions.find(inName);

	if (it == inXcryptions.end())
		return NULL;
	else
		return it->second;
}

static const string scStdCF = "StdCF";
EStatusCode DecryptionHelper::Setup(PDFParser* inParser, const string& inPassword) {
	mSupportsDecryption = false;
	mFailedPasswordVerification = false;
	mDidSucceedOwnerPasswordVerification = false;

	// setup encrypted flag through the existance of encryption dict
	PDFObjectCastPtr<PDFDictionary> encryptionDictionary(inParser->QueryDictionaryObject(inParser->GetTrailer(), "Encrypt"));
	mIsEncrypted = encryptionDictionary.GetPtr() != NULL;

	do {
		if (!mIsEncrypted)
			break;

		PDFObjectCastPtr<PDFName> filter(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "Filter"));
		if (!filter || filter->GetValue() != "Standard") {
			// Supporting only standard filter
			if(!filter)
				TRACE_LOG("DecryptionHelper::Setup, no filter defined");
			else
				TRACE_LOG1("DecryptionHelper::Setup, Only Standard encryption filter is supported. Unsupported filter encountered - %s",filter->GetValue().substr(0, MAX_TRACE_SIZE - 200).c_str());
			break;
		}

		RefCountPtr<PDFObject> v(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "V"));
		if (!v) {
			mV = 0;
		} else {
			ParsedPrimitiveHelper vHelper(v.GetPtr());
			if (!vHelper.IsNumber())
				break;
			mV = (unsigned int)vHelper.GetAsInteger();
		}

		// supporting versions 1,2 and 4
		if (mV != 1 && mV != 2 && mV != 4) {
			TRACE_LOG1("DecryptionHelper::Setup, Only 1 and 2 are supported values for V. Unsupported filter encountered - %d", mV);
			break;
		}

		RefCountPtr<PDFObject> revision(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "R"));
		if (!revision) {
			break;
		}
		else {
			ParsedPrimitiveHelper revisionHelper(revision.GetPtr());
			if (!revisionHelper.IsNumber())
				break;
			mRevision = (unsigned int)revisionHelper.GetAsInteger();
		}

		RefCountPtr<PDFObject> o(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "O"));
		if (!o) {
			break;
		}
		else {
			ParsedPrimitiveHelper oHelper(o.GetPtr());
			mO = XCryptionCommon::stringToByteList(oHelper.ToString());
		}

		RefCountPtr<PDFObject> u(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "U"));
		if (!u) {
			break;
		}
		else {
			ParsedPrimitiveHelper uHelper(u.GetPtr());
			mU = XCryptionCommon::stringToByteList(uHelper.ToString());
		}

		RefCountPtr<PDFObject> p(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "P"));
		if (!p) {
			break;
		}
		else {
			ParsedPrimitiveHelper pHelper(p.GetPtr());
			if (!pHelper.IsNumber())
				break;
			mP = pHelper.GetAsInteger();
		}

		PDFObjectCastPtr<PDFBoolean> encryptMetadata(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "EncryptMetadata"));
		if (!encryptMetadata) {
			mEncryptMetaData = true;
		}
		else {
			mEncryptMetaData = encryptMetadata->GetValue();
		}

		// grab file ID from trailer
		mFileIDPart1 = ByteList();
		PDFObjectCastPtr<PDFArray> idArray(inParser->QueryDictionaryObject(inParser->GetTrailer(), "ID"));
		if (!!idArray && idArray->GetLength() > 0) {
			RefCountPtr<PDFObject> idPart1Object(inParser->QueryArrayObject(idArray.GetPtr(), 0));
			if (!!idPart1Object) {
				ParsedPrimitiveHelper idPart1ObjectHelper(idPart1Object.GetPtr());
				mFileIDPart1 = XCryptionCommon::stringToByteList(idPart1ObjectHelper.ToString());
			}
		}


		RefCountPtr<PDFObject> length(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "Length"));
		if (!length) {
			mLength = 40 / 8;
		}
		else {
			mLength = ComputeLength(length.GetPtr());
		}

		// Setup crypt filters, or a default filter
		if (mV == 4) {
			// multiple xcryptions. read crypt filters, determine which does what
			PDFObjectCastPtr<PDFDictionary> cryptFilters(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "CF"));
			if (!!cryptFilters) {
				MapIterator<PDFNameToPDFObjectMap>  cryptFiltersIt = cryptFilters->GetIterator();

				// read crypt filters
				while (cryptFiltersIt.MoveNext())
				{
					PDFObjectCastPtr<PDFDictionary> cryptFilter(cryptFiltersIt.GetValue());
					if (!!cryptFilter) {
						PDFObjectCastPtr<PDFName> cfmName(inParser->QueryDictionaryObject(cryptFilter.GetPtr(), "CFM"));
						RefCountPtr<PDFObject> lengthObject(inParser->QueryDictionaryObject(cryptFilter.GetPtr(), "Length"));
						unsigned int length = !lengthObject ? mLength : ComputeLength(lengthObject.GetPtr());

						XCryptionCommon* encryption = new XCryptionCommon();
						encryption->Setup(cfmName->GetValue() == "AESV2"); // singe xcryptions are always RC4
						encryption->SetupInitialEncryptionKey(
							inPassword,
							mRevision,
							length,
							mO,
							mP,
							mFileIDPart1,
							mEncryptMetaData);
						mXcrypts.insert(StringToXCryptionCommonMap::value_type(cryptFiltersIt.GetKey()->GetValue(), encryption));
					}
				}
				

				PDFObjectCastPtr<PDFName> streamsFilterName(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "StmF"));
				PDFObjectCastPtr<PDFName> stringsFilterName(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "StrF"));
				mXcryptStreams = GetFilterForName(mXcrypts, !streamsFilterName ? "Identity": streamsFilterName->GetValue());
				mXcryptStrings = GetFilterForName(mXcrypts, !stringsFilterName ? "Identity" : stringsFilterName->GetValue());
				mXcryptAuthentication = GetFilterForName(mXcrypts, scStdCF);
			}

		}
		else {
			// single xcryption, use as the single encryption source
			XCryptionCommon* defaultEncryption = new XCryptionCommon();
			defaultEncryption->Setup(false); // single xcryptions are always RC4
			defaultEncryption->SetupInitialEncryptionKey(
				inPassword,
				mRevision,
				mLength,
				mO,
				mP,
				mFileIDPart1,
				mEncryptMetaData);

			mXcrypts.insert(StringToXCryptionCommonMap::value_type(scStdCF, defaultEncryption));
			mXcryptStreams = defaultEncryption;
			mXcryptStrings = defaultEncryption;
			mXcryptAuthentication = defaultEncryption;
		}

		// authenticate password, try to determine if user or owner
		ByteList password = XCryptionCommon::stringToByteList(inPassword);
		mDidSucceedOwnerPasswordVerification = AuthenticateOwnerPassword(password);
		mFailedPasswordVerification = !mDidSucceedOwnerPasswordVerification && !AuthenticateUserPassword(password);


		mSupportsDecryption = true;
	}while(false);

	return eSuccess;
}

bool DecryptionHelper::IsEncrypted() const{
	return mIsEncrypted;
}

bool DecryptionHelper::SupportsDecryption() const {
	return mSupportsDecryption;
}

bool DecryptionHelper::CanDecryptDocument() const {
	return mSupportsDecryption && !mFailedPasswordVerification;
}

bool DecryptionHelper::DidFailPasswordVerification() const {
	return mFailedPasswordVerification;
}

bool DecryptionHelper::DidSucceedOwnerPasswordVerification() const {
	return mDidSucceedOwnerPasswordVerification;
}

static const string scEcnryptionKeyMetadataKey = "DecryptionHelper.EncryptionKey";

IByteReader* DecryptionHelper::CreateDecryptionFilterForStream(PDFStreamInput* inStream, IByteReader* inToWrapStream) {
	if (!IsEncrypted() || !CanDecryptDocument())
		return NULL;
	
	void* savedEcnryptionKey = inStream->GetMetadata(scEcnryptionKeyMetadataKey);
	if (savedEcnryptionKey) {
		return CreateDecryptionReader(inToWrapStream, *((ByteList*)savedEcnryptionKey));
	}
	else 
		return NULL;
}

std::string DecryptionHelper::DecryptString(const std::string& inStringToDecrypt) {
	if (!IsEncrypted() || !CanDecryptDocument() || !mXcryptStrings)
		return inStringToDecrypt;

	IByteReader* decryptStream = CreateDecryptionReader(new InputStringStream(inStringToDecrypt), mXcryptStrings->GetCurrentObjectKey());
	if (decryptStream) {
		OutputStringBufferStream outputStream;
		OutputStreamTraits traits(&outputStream);
		traits.CopyToOutputStream(decryptStream);

		return outputStream.ToString();
	}
	else
		return inStringToDecrypt;
}

void DecryptionHelper::OnObjectStart(long long inObjectID, long long inGenerationNumber) {
	if (!IsEncrypted() || !CanDecryptDocument())
		return;

	StringToXCryptionCommonMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it) {
		it->second->OnObjectStart(inObjectID, inGenerationNumber);
	}
}
void DecryptionHelper::OnObjectEnd(PDFObject* inObject) {
	if (!IsEncrypted() || !CanDecryptDocument())
		return;

	// for streams, retain the encryption key with them, so i can later decrypt them when needed
	// Gal: TBD here is to see if the stream has a crypt filter, and use that instead!!!
	if ((inObject->GetType() == PDFObject::ePDFObjectStream) && mXcryptStreams) {
		ByteList* savedKey = new ByteList(mXcryptStreams->GetCurrentObjectKey());
		inObject->SetMetadata(scEcnryptionKeyMetadataKey, savedKey);
	}

	StringToXCryptionCommonMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it) {
		it->second->OnObjectEnd();
	}
}

IByteReader* DecryptionHelper::CreateDecryptionReader(IByteReader* inSourceStream, const ByteList& inEncryptionKey) {
	return new InputRC4XcodeStream(inSourceStream, inEncryptionKey);
}


bool DecryptionHelper::AuthenticateUserPassword(const ByteList& inPassword) {
	if (!mXcryptAuthentication)
		return true;
	return mXcryptAuthentication->algorithm3_6(mRevision,
						mLength,
						inPassword,
						mO,
						mP,
						mFileIDPart1,
						mEncryptMetaData,
						mU);
}

bool DecryptionHelper::AuthenticateOwnerPassword(const ByteList& inPassword) {
	if (!mXcryptAuthentication)
		return true;

	return mXcryptAuthentication->algorithm3_7(mRevision,
						mLength,
						inPassword,
						mO,
						mP,
						mFileIDPart1,
						mEncryptMetaData,
						mU);
}

unsigned int DecryptionHelper::GetLength() const
{
	return mLength;
}

unsigned int DecryptionHelper::GetV() const
{
	return mV;
}

unsigned int DecryptionHelper::GetRevision() const
{
	return mRevision;
}

long long DecryptionHelper::GetP() const
{
	return mP;
}

bool DecryptionHelper::GetEncryptMetaData() const
{
	return mEncryptMetaData;
}

const ByteList& DecryptionHelper::GetFileIDPart1() const
{
	return mFileIDPart1;
}

const ByteList& DecryptionHelper::GetO() const
{
	return mO;
}

const ByteList& DecryptionHelper::GetU() const
{
	return mU;
}

const ByteList& DecryptionHelper::GetInitialEncryptionKey() const
{
	return mXcryptAuthentication->GetInitialEncryptionKey();
}