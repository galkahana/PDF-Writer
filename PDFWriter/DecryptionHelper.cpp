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
#include "InputAESDecodeStream.h"
#include "Trace.h"
#include "Deletable.h"
#include "ByteList.h"
#include "XCryptionCommon.h"
#include "XCryptionCommon2_0.h"
#include <memory>
#include <algorithm>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

DecryptionHelper::DecryptionHelper(void)
{
	Reset();
}

void DecryptionHelper::Release() {
	StringToXCryptorMap::iterator it = mXcrypts.begin();
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
	mParser = NULL;
	mDecryptionPauseLevel = 0;
	Release();
}

unsigned int ComputeByteLength(PDFObject* inLengthObject) {
	// The bit length of the file encryption key shall be a multiple of 8 in the range of 40 to 256 bits. This function returns the length in bytes.
	ParsedPrimitiveHelper lengthHelper(inLengthObject);
	unsigned int value = lengthHelper.IsNumber() ? (unsigned int)lengthHelper.GetAsInteger() : 40;
	return value < 40 ? std::max(value, (unsigned int)5) : value / 8; // this small check here is based on some errors i saw, where the length was given in bytes instead of bits
}

XCryptor* GetFilterForName(const StringToXCryptorMap& inXcryptions, const string& inName) {
	StringToXCryptorMap::const_iterator it = inXcryptions.find(inName);

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
	mParser = inParser;

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
				TRACE_LOG1("DecryptionHelper::Setup, Only Standard encryption handler is supported. Unsupported filter value encountered - %s",filter->GetValue().substr(0, MAX_TRACE_SIZE - 200).c_str());
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

		// supporting versions 1, 2, 4 and 5
#ifdef PDFHUMMUS_NO_OPENSSL		
		if (mV == 5) {
			TRACE_LOG("DecryptionHelper::Setup, V value of 5 is only supported with OpenSSL installed. Please install OpenSSL and make sure PDFHUMMUS_NO_OPENSSL is not defined and try again.");
			break;
		}
#endif
		if (mV != 1 && mV != 2 && mV != 4 && mV != 5) {
			TRACE_LOG1("DecryptionHelper::Setup, Only 1, 2, 4 and 5 are supported values for V. Unsupported V value encountered - %d", mV);
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

		size_t ouLength = mRevision <= 4 ? 32 : 48; // R <= 4 is 32 bytes, R >= 5 is 48 bytes
		size_t oEuELength = 32;

		RefCountPtr<PDFObject> o(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "O"));
		if (!o) {
			break;
		}
		else {
			ParsedPrimitiveHelper oHelper(o.GetPtr());
			mO = stringToByteList(oHelper.ToString());
			if(mO.size() > ouLength) {
				// some wiseasses might use longer value then expected and this causes trouble with auth. trim.
				mO = substr(mO, 0, ouLength);
			}

		}

		RefCountPtr<PDFObject> u(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "U"));
		if (!u) {
			break;
		}
		else {
			ParsedPrimitiveHelper uHelper(u.GetPtr());
			mU = stringToByteList(uHelper.ToString());
			if(mU.size() > ouLength) {
				mU = substr(mU, 0, ouLength);
			}

		}

		RefCountPtr<PDFObject> oE(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "OE"));
		if (!oE) {
			if (mV >= 5) {
				TRACE_LOG("DecryptionHelper::Setup, OE not defined in encryption dictionary, but V > 5. This is unexpected. breaking");
				break;
			}
		}
		else {
			ParsedPrimitiveHelper oEHelper(oE.GetPtr());
			mOE = stringToByteList(oEHelper.ToString());
			if(mOE.size() > oEuELength) {
				mOE = substr(mOE, 0, oEuELength);
			}
		}	

		RefCountPtr<PDFObject> uE(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "UE"));
		if (!uE) {
			if (mV >= 5) {
				TRACE_LOG("DecryptionHelper::Setup, UE not defined in encryption dictionary, but V > 5. This is unexpected. breaking");
				break;
			}
		} else {
			ParsedPrimitiveHelper uEHelper(uE.GetPtr());
			mUE = stringToByteList(uEHelper.ToString());
			if(mUE.size() > oEuELength) {
				mUE = substr(mUE, 0, oEuELength);
			}
		}

		RefCountPtr<PDFObject> perms(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "Perms"));
		if (!perms) {
			if (mV >= 5) {
				TRACE_LOG("DecryptionHelper::Setup, Perms not defined in encryption dictionary, but V > 5. This is unexpected. breaking");
				break;
			}
		}
		else {
			ParsedPrimitiveHelper permsHelper(perms.GetPtr());
			mPerms = stringToByteList(permsHelper.ToString());
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
				mFileIDPart1 = stringToByteList(idPart1ObjectHelper.ToString());
			}
		}


		RefCountPtr<PDFObject> length(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "Length"));
		if (!length) {
			mLength = 40 / 8;
		}
		else {
			mLength = ComputeByteLength(length.GetPtr());
		}

		ByteList password = stringToByteList(inPassword);

		// Setup crypt filters, or a default filter
		if (mV == 4 || mV == 5) {
			// multiple xcryptions. read crypt filters, determine which does what
			PDFObjectCastPtr<PDFDictionary> cryptFilters(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "CF"));
			if (!!cryptFilters) {
				MapIterator<PDFNameToPDFObjectMap>  cryptFiltersIt = cryptFilters->GetIterator();

				// read crypt filters
				EStatusCode status = eSuccess;
				while (cryptFiltersIt.MoveNext() && status == eSuccess)
				{
					PDFObjectCastPtr<PDFDictionary> cryptFilter;
					// A little caveat of those smart ptrs need to be handled here
					// make sure to pass the pointer after init...otherwise cast wont do addref
					// and object will be released
					cryptFilter = cryptFiltersIt.GetValue();
					if (!!cryptFilter) {
						PDFObjectCastPtr<PDFName> cfmNameObject(inParser->QueryDictionaryObject(cryptFilter.GetPtr(), "CFM"));
						RefCountPtr<PDFObject> lengthObject(inParser->QueryDictionaryObject(cryptFilter.GetPtr(), "Length"));
						unsigned int cryptLength = !lengthObject ? mLength : ComputeByteLength(lengthObject.GetPtr());
						// setup encryption method (based on cfmName) and key (based on length)
						string cfmName = cfmNameObject->GetValue();
						if(cfmName != "AESV2" && cfmName != "AESV3" && cfmName != "RC4") {
							continue; // probably "None"". could also be unsupported. in any case, ignore and this means that this crypt filter will not encrypt
						}
						EXCryptorAlgo xCryptorAlgo = cfmName == "AESV3" ? eAESV3 : (cfmName == "AESV2" ? eAESV2 : eRC4);

						// retrieve encryption key, based on this crypt filter params
						ByteList fileEncryptionKey;
						if(xCryptorAlgo == eAESV3) {
#ifdef PDFHUMMUS_NO_OPENSSL		
							TRACE_LOG("DecryptionHelper::Setup, AESV3 is only supported with OpenSSL installed. Please install OpenSSL and make sure PDFHUMMUS_NO_OPENSSL is not defined and try again.");
							status = eFailure;
							break;
#else
							XCryptionCommon2_0 xcryption2_0;
							fileEncryptionKey = xcryption2_0.RetrieveFileEncryptionKey(
								password,
								mO,
								mU,
								mOE,
								mUE);
#endif
						} else {
							XCryptionCommon xcryption;
							fileEncryptionKey = xcryption.RetrieveFileEncryptionKey(
								password,
								mRevision,
								cryptLength,
								mO,
								mP,
								mFileIDPart1,
								mEncryptMetaData);						

						}

						
						XCryptor* encryption = new XCryptor(xCryptorAlgo, fileEncryptionKey);
						mXcrypts.insert(StringToXCryptorMap::value_type(cryptFiltersIt.GetKey()->GetValue(), encryption));
					}
				}
				if(status != eSuccess) {
					TRACE_LOG("DecryptionHelper::Setup, Failed to read crypt filters from encryption dictionary. breaking");
					break;
				}
				

				PDFObjectCastPtr<PDFName> streamsFilterName(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "StmF"));
				PDFObjectCastPtr<PDFName> stringsFilterName(inParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "StrF"));
				mXcryptStreams = GetFilterForName(mXcrypts, !streamsFilterName ? "Identity": streamsFilterName->GetValue());
				mXcryptStrings = GetFilterForName(mXcrypts, !stringsFilterName ? "Identity" : stringsFilterName->GetValue());
			}

		} else {
			XCryptionCommon xcryption;
			ByteList fileEncryptionKey = xcryption.RetrieveFileEncryptionKey(
				password,
				mRevision,
				mLength,
				mO,
				mP,
				mFileIDPart1,
				mEncryptMetaData);

			// single xcryption, use as the single encryption method
			XCryptor* defaultEncryption = new XCryptor(eRC4, fileEncryptionKey); //non (or rather - pre) version 4 are RC4 always and not using AES
			mXcrypts.insert(StringToXCryptorMap::value_type(scStdCF, defaultEncryption));
			mXcryptStreams = defaultEncryption;
			mXcryptStrings = defaultEncryption;
		}

		// authenticate password, try to determine if user or owner
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

bool HasCryptFilterDefinition(PDFParser* inParser, PDFStreamInput* inStream) {
	RefCountPtr<PDFDictionary> streamDictionary(inStream->QueryStreamDictionary());

	// check if stream has a crypt filter
	RefCountPtr<PDFObject> filterObject(inParser->QueryDictionaryObject(streamDictionary.GetPtr(), "Filter"));
	if (!filterObject)
	{
		// no filter, so stop here
		return false;
	}

	if (filterObject->GetType() == PDFObject::ePDFObjectArray)
	{
		PDFArray* filterObjectArray = (PDFArray*)filterObject.GetPtr();
		bool foundCrypt = false;
		for (unsigned long i = 0; i < filterObjectArray->GetLength() && !foundCrypt; ++i)
		{
			PDFObjectCastPtr<PDFName> filterObjectItem(filterObjectArray->QueryObject(i));
			if (!filterObjectItem)
			{
				// error
				break;
			}
			foundCrypt = filterObjectItem->GetValue() == "Crypt";
		}
		return foundCrypt;
	}
	else if (filterObject->GetType() == PDFObject::ePDFObjectName)
	{
		return ((PDFName*)(filterObject.GetPtr()))->GetValue() == "Crypt";
	}
	else
		return false; //???
}


IByteReader* DecryptionHelper::CreateDefaultDecryptionFilterForStream(PDFStreamInput* inStream, IByteReader* inToWrapStream) {
	// This will create a decryption filter for streams that dont have their own defined crypt filters. null for no decryption filter
	if (!IsEncrypted() || !CanDecryptDocument() || HasCryptFilterDefinition(mParser, inStream) || !mXcryptStreams)
		return NULL;
	
	IDeletable* savedEcnryptionKey = inStream->GetMetadata(scEcnryptionKeyMetadataKey);
	if (savedEcnryptionKey) {
		return CreateDecryptionReader(inToWrapStream, *(((Deletable<ByteList>*)savedEcnryptionKey)->GetPtr()), mXcryptStreams->GetIsUsingAES());
	}
	else 
		return NULL;
}

IByteReader*  DecryptionHelper::CreateDecryptionFilterForStream(PDFStreamInput* inStream, IByteReader* inToWrapStream, const std::string& inCryptName) {
	// note that here the original stream is returned instead of null
	if (!IsEncrypted() || !CanDecryptDocument())
		return inToWrapStream;

	IDeletable* savedEcnryptionKey = inStream->GetMetadata(scEcnryptionKeyMetadataKey);
	if (!savedEcnryptionKey) {
		// sign for no encryption here
		return inToWrapStream;
	}
	XCryptor* xcryption = GetFilterForName(mXcrypts, inCryptName);

	if (xcryption && savedEcnryptionKey) {
		return CreateDecryptionReader(inToWrapStream, *(((Deletable<ByteList>*)savedEcnryptionKey)->GetPtr()), xcryption->GetIsUsingAES());
	}
	else
		return inToWrapStream;

}

bool DecryptionHelper::IsDecrypting() {
	return IsEncrypted() && CanDecryptDocument() && mDecryptionPauseLevel == 0;
}

std::string DecryptionHelper::DecryptString(const std::string& inStringToDecrypt) {
	if (!IsDecrypting() || !mXcryptStrings)
		return inStringToDecrypt;

	IByteReader* decryptStream = CreateDecryptionReader(new InputStringStream(inStringToDecrypt), mXcryptStrings->GetCurrentObjectKey(), mXcryptStrings->GetIsUsingAES());
	if (decryptStream) {
		OutputStringBufferStream outputStream;
		OutputStreamTraits traits(&outputStream);
		traits.CopyToOutputStream(decryptStream);

		delete decryptStream;
		return outputStream.ToString();
	}
	else
		return inStringToDecrypt;
}

void DecryptionHelper::OnObjectStart(long long inObjectID, long long inGenerationNumber) {
	StringToXCryptorMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it) {
		it->second->OnObjectStart(inObjectID, inGenerationNumber);
	}
}


XCryptor* DecryptionHelper::GetCryptForStream(PDFStreamInput* inStream) {
	// Get crypt for stream will return the right crypt filter thats supposed to be used for stream
	// whether its the default stream encryption or a specific filter defined in the stream
	// not the assumption (well, one that's all over) that if the name is not found in the CF dict, it
	// will be "identity" which is the same as providing NULL as the XCryptor return value

	if (HasCryptFilterDefinition(mParser, inStream)) {
		// find position of crypt filter, and get the name of the crypt filter from the decodeParams
		RefCountPtr<PDFDictionary> streamDictionary(inStream->QueryStreamDictionary());

		RefCountPtr<PDFObject> filterObject(mParser->QueryDictionaryObject(streamDictionary.GetPtr(), "Filter"));
		if (filterObject->GetType() == PDFObject::ePDFObjectArray)
		{
			PDFArray* filterObjectArray = (PDFArray*)filterObject.GetPtr();
			unsigned long i = 0;
			for (; i < filterObjectArray->GetLength(); ++i)
			{
				PDFObjectCastPtr<PDFName> filterObjectItem(filterObjectArray->QueryObject(i));
				if (!filterObjectItem || filterObjectItem->GetValue() == "Crypt")
					break;
			}
			if (i < filterObjectArray->GetLength()) {
				PDFObjectCastPtr<PDFArray> decodeParams(mParser->QueryDictionaryObject(streamDictionary.GetPtr(), "DecodeParms"));
				if (!decodeParams)
					return mXcryptStreams;
				// got index, look for the name in the decode params array
				PDFObjectCastPtr<PDFDictionary> decodeParamsItem((mParser->QueryArrayObject(decodeParams.GetPtr(), i)));
				if (!decodeParamsItem)
					return mXcryptStreams;

				PDFObjectCastPtr<PDFName> cryptFilterName(mParser->QueryDictionaryObject(decodeParamsItem.GetPtr(), "Name"));
				return GetFilterForName(mXcrypts, cryptFilterName->GetValue());

			}
			else
				return mXcryptStreams; // this shouldn't realy happen
		}
		else if (filterObject->GetType() == PDFObject::ePDFObjectName)
		{
			// has to be crypt filter, look for the name in decode params
			PDFObjectCastPtr<PDFDictionary> decodeParamsItem((mParser->QueryDictionaryObject(streamDictionary.GetPtr(), "DecodeParms")));
			if (!decodeParamsItem)
				return mXcryptStreams;

			PDFObjectCastPtr<PDFName> cryptFilterName(mParser->QueryDictionaryObject(decodeParamsItem.GetPtr(), "Name"));
			return GetFilterForName(mXcrypts, cryptFilterName->GetValue());
		}
		else
			return mXcryptStreams; // ???
	}
	else {
		return mXcryptStreams;
	}
}

void DecryptionHelper::OnObjectEnd(PDFObject* inObject) {
	if (inObject == NULL)
		return;
	
	// for streams, retain the encryption key with them, so i can later decrypt them when needed
	if ((inObject->GetType() == PDFObject::ePDFObjectStream) && IsDecrypting()) {
		XCryptor* streamCryptFilter = GetCryptForStream((PDFStreamInput*)inObject);
		if (streamCryptFilter) {
			ByteList* savedKey = new ByteList(streamCryptFilter->GetCurrentObjectKey());
			inObject->SetMetadata(scEcnryptionKeyMetadataKey,new Deletable<ByteList>(savedKey));
		}
	}

	StringToXCryptorMap::iterator it = mXcrypts.begin();
	for (; it != mXcrypts.end(); ++it) {
		it->second->OnObjectEnd();
	}
}

IByteReader* DecryptionHelper::CreateDecryptionReader(IByteReader* inSourceStream, const ByteList& inEncryptionKey, bool inIsUsingAES) {
	if (inIsUsingAES)
		return new InputAESDecodeStream(inSourceStream, inEncryptionKey);
	else
		return new InputRC4XcodeStream(inSourceStream, inEncryptionKey);
}


bool DecryptionHelper::AuthenticateUserPassword(const ByteList& inPassword) {
	if(mV >= 5) {
#ifdef PDFHUMMUS_NO_OPENSSL
		TRACE_LOG("DecryptionHelper::AuthenticateUserPassword, V value of 5 is only supported with OpenSSL installed. Please install OpenSSL and make sure PDFHUMMUS_NO_OPENSSL is not defined and try again.");
		return false;
#else
		XCryptionCommon2_0 xcryption2_0;
		return xcryption2_0.AuthenticateUserPassword(
						inPassword,
						mU
					);
#endif
	} else {
		XCryptionCommon xcryption;
		return xcryption.AuthenticateUserPassword(
							inPassword,
							mRevision,
							mLength,
							mO,
							mP,
							mFileIDPart1,
							mEncryptMetaData,
							mU);

	}
}

bool DecryptionHelper::AuthenticateOwnerPassword(const ByteList& inPassword) {
	if(mV >= 5) {
#ifdef PDFHUMMUS_NO_OPENSSL
		TRACE_LOG("DecryptionHelper:AuthenticateOwnerPassword, V value of 5 is only supported with OpenSSL installed. Please install OpenSSL and make sure PDFHUMMUS_NO_OPENSSL is not defined and try again.");
		return false;
#else
		XCryptionCommon2_0 xcryption2_0;
		return xcryption2_0.AuthenticateOwnerPassword(
						inPassword,
						mO,
						mU
					);
#endif
	} else {
		XCryptionCommon xcryption;
		return xcryption.AuthenticateOwnerPassword(
							inPassword,
							mRevision,
							mLength,
							mO,
							mP,
							mFileIDPart1,
							mEncryptMetaData,
							mU);

	}
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

const ByteList& DecryptionHelper::GetOE() const
{
	return mOE;
}

const ByteList& DecryptionHelper::GetUE() const
{
	return mUE;
}

const ByteList& DecryptionHelper::GetPerms() const
{
	return mPerms;
}


void DecryptionHelper::PauseDecryption() {
	++mDecryptionPauseLevel;
}

void DecryptionHelper::ReleaseDecryption() {
	--mDecryptionPauseLevel;
}

const StringToXCryptorMap& DecryptionHelper::GetXcrypts() const {
	return mXcrypts;
}

XCryptor* DecryptionHelper::GetStreamXcrypt() const {
	return mXcryptStreams;
}

XCryptor* DecryptionHelper::GetStringXcrypt() const {
	return mXcryptStrings;
}
