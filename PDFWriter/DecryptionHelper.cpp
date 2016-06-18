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
#include "MD5Generator.h"
#include "ParsedPrimitiveHelper.h"
#include "RefCountPtr.h"
#include "OutputStringBufferStream.h"
#include "InputRC4XcodeStream.h"
#include "RC4.h"

#include <stdint.h>
#include <algorithm>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

const Byte scPaddingFiller[] = { 0x28,0xBF,0x4E,0x5E,0x4E,0x75,0x8A,0x41,0x64,0x00,0x4E,0x56,0xFF,0xFA,0x01,0x08,0x2E,0x2E,0x00,0xB6,0xD0,0x68,0x3E,0x80,0x2F,0x0C,0xA9,0xFE,0x64,0x53,0x69,0x7A };
DecryptionHelper::DecryptionHelper(void)
{
	for (int i = 0; i < 32; ++i)
		mPaddingFiller.push_back(scPaddingFiller[i]);

}

DecryptionHelper::~DecryptionHelper(void)
{
}


EStatusCode DecryptionHelper::Setup(PDFParser* inParser, const std::string& inPassword) {
	mParser = inParser;
	mSupportsDecryption = false;
	mFailedPasswordVerification = false;
	mDidSucceedOwnerPasswordVerification = false;

	// setup encrypted flag
	PDFObjectCastPtr<PDFDictionary> encryptionDictionary(mParser->QueryDictionaryObject(mParser->GetTrailer(), "Encrypt"));
	mIsEncrypted = encryptionDictionary.GetPtr() != NULL;

	do {
		if (!mIsEncrypted)
			break;

		PDFObjectCastPtr<PDFName> filter(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "Filter"));
		if (!filter || filter->GetValue() != "Standard") 
			// Supporting only standard filter
			break;

		RefCountPtr<PDFObject> v(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "V"));
		if (!v) {
			mV = 0;
		} else {
			ParsedPrimitiveHelper vHelper(v.GetPtr());
			if (!vHelper.IsNumber())
				break;
			mV = (unsigned int)vHelper.GetAsInteger();
		}

		// supporting only up to 3 (4 uses CF, StmF and StrF)
		if (mV > 3)
			break;

		RefCountPtr<PDFObject> length(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "Length"));
		if (!length) {
			mLength = 40/8;
		}
		else {
			ParsedPrimitiveHelper lengthHelper(length.GetPtr());
			if (!lengthHelper.IsNumber())
				break;
			mLength = (unsigned int)lengthHelper.GetAsInteger()/8;
		}

		RefCountPtr<PDFObject> revision(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "R"));
		if (!revision) {
			break;
		}
		else {
			ParsedPrimitiveHelper revisionHelper(revision.GetPtr());
			if (!revisionHelper.IsNumber())
				break;
			mRevision = (unsigned int)revisionHelper.GetAsInteger();
		}

		RefCountPtr<PDFObject> o(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "O"));
		if (!o) {
			break;
		}
		else {
			ParsedPrimitiveHelper oHelper(o.GetPtr());
			mO = stringToByteList(oHelper.ToString());
		}

		RefCountPtr<PDFObject> u(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "U"));
		if (!u) {
			break;
		}
		else {
			ParsedPrimitiveHelper uHelper(u.GetPtr());
			mU = stringToByteList(uHelper.ToString());
		}

		RefCountPtr<PDFObject> p(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "P"));
		if (!p) {
			break;
		}
		else {
			ParsedPrimitiveHelper pHelper(p.GetPtr());
			if (!pHelper.IsNumber())
				break;
			mP = pHelper.GetAsInteger();
		}

		PDFObjectCastPtr<PDFBoolean> encryptMetadata(mParser->QueryDictionaryObject(encryptionDictionary.GetPtr(), "EncryptMetadata"));
		if (!encryptMetadata) {
			mEncryptMetaData = true;
		}
		else {
			mEncryptMetaData = encryptMetadata->GetValue();
		}

		// grab file ID from trailer
		mFileIDPart1 = ByteList();
		PDFObjectCastPtr<PDFArray> idArray(mParser->QueryDictionaryObject(mParser->GetTrailer(), "ID"));
		if (!!idArray && idArray->GetLength() > 0) {
			RefCountPtr<PDFObject> idPart1Object(mParser->QueryArrayObject(idArray.GetPtr(), 0));
			if (!!idPart1Object) {
				ParsedPrimitiveHelper idPart1ObjectHelper(idPart1Object.GetPtr());
				mFileIDPart1 = stringToByteList(idPart1ObjectHelper.ToString());
			}
		}

		// determing if using AES (PDF >= 1.6)
		mUsingAES = (inParser->GetPDFLevel() >= 1.6);

		if (mUsingAES) // unsupported yet!
			break;

		// authenticate password, try to determine if user or owner
		ByteList password = stringToByteList(inPassword);

		mDidSucceedOwnerPasswordVerification = AuthenticateOwnerPassword(password);
		mFailedPasswordVerification = !mDidSucceedOwnerPasswordVerification && !AuthenticateUserPassword(password);

		if(!mFailedPasswordVerification)
			mEncryptionKey = ComputeEncryptionKey(password);

		mSupportsDecryption = true;
	}while(false);


	// TODO: make sure i only pass through decryption when really possible!

	return EStatusCode::eSuccess;
}

bool DecryptionHelper::IsEncrypted() {
	return mIsEncrypted;
}

bool DecryptionHelper::SupportsDecryption() {
	return mSupportsDecryption;
}

bool DecryptionHelper::CanDecryptDocument() {
	return mSupportsDecryption && !mFailedPasswordVerification;
}

bool DecryptionHelper::DidFailPasswordVerification() {
	return mFailedPasswordVerification;
}

bool DecryptionHelper::DidSucceedOwnerPasswordVerification() {
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
	if (!IsEncrypted() || !CanDecryptDocument())
		return inStringToDecrypt;

	IByteReader* decryptStream = CreateDecryptionReader(new InputStringStream(inStringToDecrypt), mEncryptionKeysStack.size() > 0 ? mEncryptionKeysStack.back():ByteList());
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

	mEncryptionKeysStack.push_back(ComputeEncryptionKeyForObject((ObjectIDType)inObjectID, (unsigned long)inGenerationNumber));
}
void DecryptionHelper::OnObjectEnd(PDFObject* inObject) {
	if (!IsEncrypted() || !CanDecryptDocument())
		return;

	// for streams, retain the encryption key with them, so i can later decrypt them when needed
	if (inObject->GetType() == PDFObject::ePDFObjectStream) {
		ByteList* savedKey = new ByteList(mEncryptionKeysStack.back());
		inObject->SetMetadata(scEcnryptionKeyMetadataKey, savedKey);
	}

	mEncryptionKeysStack.pop_back();

}

IByteReader* DecryptionHelper::CreateDecryptionReader(IByteReader* inSourceStream, const ByteList& inEncryptionKey) {
	return new InputRC4XcodeStream(inSourceStream, inEncryptionKey);
}


const Byte scAESSuffix[] = { 0x73, 0x41, 0x63, 0x54 };
ByteList DecryptionHelper::algorithm3_1(ObjectIDType inObjectNumber,
											unsigned long inGenerationNumber,
											const ByteList& inEncryptionKey,
											bool inIsUsingAES) {
	MD5Generator md5;
	ByteList result = inEncryptionKey;
	Byte buffer;
	LongBufferSizeType outputKeyLength = std::min(inEncryptionKey.size() + 5,16U);

	buffer = inObjectNumber & 0xff;
	result.push_back(buffer);
	inObjectNumber >>= 8;
	buffer = inObjectNumber & 0xff;
	result.push_back(buffer);
	inObjectNumber >>= 8;
	buffer = inObjectNumber & 0xff;
	result.push_back(buffer);

	buffer = inGenerationNumber & 0xff;
	result.push_back(buffer);
	inGenerationNumber >>= 8;
	buffer = inGenerationNumber & 0xff;
	result.push_back(buffer);

	if (inIsUsingAES) {
		for (int i = 0; i < 4; ++i) {
			result.push_back(scAESSuffix[i]);
		}
	}
	md5.Accumulate(result);

	return substr(md5.ToString(),0, outputKeyLength);
}

ByteList DecryptionHelper::ComputeEncryptionKeyForObject(ObjectIDType inObjectNumber, 
														unsigned long inGenerationNumber) {
	return algorithm3_1(inObjectNumber, inGenerationNumber, mEncryptionKey, mUsingAES);
}

const Byte scFixedEnd[] = { 0xFF,0xFF,0xFF,0xFF };
ByteList DecryptionHelper::algorithm3_2(unsigned int inRevision,
											unsigned int inLength,
											const ByteList& inPassword,
											const ByteList& inO,
											long long inP,
											const ByteList& inFileIDPart1,
											bool inEncryptMetaData) {
	MD5Generator md5;
	ByteList password32Chars = substr(inPassword, 0, 32);
	if (password32Chars.size() < 32)
		append(password32Chars, substr(mPaddingFiller, 0, 32 - inPassword.size()));
	uint32_t truncP = uint32_t(inP);
	Byte truncPBuffer[4];
	ByteList hashResult;
	

	md5.Accumulate(password32Chars);
	md5.Accumulate(inO);
	for (int i = 0; i < 4; ++i) {
		truncPBuffer[i] = truncP & 0xFF;
		truncP >>= 8;
	}
	md5.Accumulate(truncPBuffer,4);
	md5.Accumulate(inFileIDPart1);

	if (inRevision >= 4 && !inEncryptMetaData)
		md5.Accumulate(scFixedEnd, 4);

	hashResult = md5.ToString();

	if (inRevision >= 3) {
		for (int i = 0; i < 50; ++i) {
			MD5Generator anotherMD5;
			anotherMD5.Accumulate(substr(hashResult,0, inLength));
			hashResult = anotherMD5.ToString();

		}
	}

	return inRevision == 2 ? substr(hashResult,0, 5) : substr(hashResult,0, inLength);
}

ByteList DecryptionHelper::ComputeEncryptionKey(const ByteList& inPassword) {
	return algorithm3_2(mRevision,mLength,inPassword,mO,mP,mFileIDPart1,mEncryptMetaData);
}

ByteList DecryptionHelper::algorithm3_3(unsigned int inRevision,
										unsigned int inLength,
										const ByteList& inOwnerPassword,
										const ByteList& inUserPassword) {
	ByteList ownerPassword32Chars = add(substr(inOwnerPassword,0, 32),(inOwnerPassword.size()<32 ? substr(mPaddingFiller,0, 32 - inOwnerPassword.size()) : ByteList()));
	ByteList userPassword32Chars = add(substr(inUserPassword, 0, 32), (inUserPassword.size()<32 ? substr(mPaddingFiller, 0, 32 - inUserPassword.size()) : ByteList()));
	MD5Generator md5;
	ByteList hashResult;

	md5.Accumulate(ownerPassword32Chars);

	hashResult = md5.ToString();

	if (inRevision >= 3) {
		for (int i = 0; i < 50; ++i) {
			MD5Generator anotherMD5;
			anotherMD5.Accumulate(hashResult);
			hashResult = anotherMD5.ToString();
		}
	}

	ByteList RC4Key =  (inRevision == 2 ? substr(hashResult,0, 5) : substr(hashResult,0, inLength));

	hashResult = RC4Encode(RC4Key, userPassword32Chars);

	if (inRevision >= 3) {
		for (Byte i = 1; i <= 19; ++i) {
			ByteList newRC4Key;
			ByteList::iterator it = RC4Key.begin();
			for (; it != RC4Key.end(); ++it)
				newRC4Key.push_back((*it) ^ i);
			hashResult = RC4Encode(newRC4Key, hashResult);
		}
	}

	return hashResult;
}

ByteList DecryptionHelper::RC4Encode(const ByteList& inKey, const ByteList& inToEncode) {
	RC4 rc4(inKey);
	ByteList target;
	Byte buffer;
	ByteList::const_iterator it = inToEncode.begin();

	for (; it != inToEncode.end(); ++it) {
		buffer = rc4.DecodeNextByte((Byte)*it);
		target.push_back(buffer);
	}
	return target;
}

ByteList DecryptionHelper::algorithm3_4(unsigned int inLength,
	const ByteList& inUserPassword,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData) {
	ByteList encryptionKey = algorithm3_2(2, inLength, inUserPassword, inO, inP, inFileIDPart1, inEncryptMetaData);
	
	return RC4Encode(encryptionKey, mPaddingFiller);
}

ByteList DecryptionHelper::algorithm3_5(unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inUserPassword,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData) {
	ByteList encryptionKey = algorithm3_2(inRevision, inLength, inUserPassword, inO, inP, inFileIDPart1, inEncryptMetaData);
	MD5Generator md5;
	ByteList hashResult;

	md5.Accumulate(mPaddingFiller);
	md5.Accumulate(inFileIDPart1);
	hashResult = md5.ToString();

	hashResult = RC4Encode(encryptionKey, hashResult);

	for (Byte i = 1; i <= 19; ++i) {
		ByteList newEncryptionKey;
		ByteList::iterator it = encryptionKey.begin();
		for (; it != encryptionKey.end(); ++it)
			newEncryptionKey.push_back((*it) ^ i);
		hashResult = RC4Encode(newEncryptionKey, hashResult);
	}

	return add(hashResult,substr(mPaddingFiller,0,16));
}

bool DecryptionHelper::algorithm3_6(unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inPassword,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData,
	const ByteList inU) {
	ByteList hashResult = (inRevision == 2) ?
		algorithm3_4(inLength, inPassword, inO, inP, inFileIDPart1, inEncryptMetaData) :
		algorithm3_5(inRevision, inLength, inPassword, inO, inP, inFileIDPart1, inEncryptMetaData);

	return (inRevision == 2) ? (hashResult == inU) : (substr(hashResult,0, 16) == substr(inU,0, 16));
}

bool DecryptionHelper::algorithm3_7(unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inPassword,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData,
	const ByteList inU) {
	ByteList password32Chars = add(substr(inPassword, 0, 32), (inPassword.size()<32 ? substr(mPaddingFiller, 0, 32 - inPassword.size()) : ByteList()));
	MD5Generator md5;
	ByteList hashResult;

	md5.Accumulate(password32Chars);

	hashResult = md5.ToString();

	if (inRevision >= 3) {
		for (int i = 0; i < 50; ++i) {
			MD5Generator anotherMD5;
			anotherMD5.Accumulate(hashResult);
			hashResult = anotherMD5.ToString();
		}
	}

	ByteList RC4Key = (inRevision == 2 ? substr(hashResult,0, 5) : substr(hashResult,0, inLength));

	if (inRevision == 2) {
		hashResult = RC4Encode(RC4Key, inO);
	}
	else if (inRevision >= 3) {
		hashResult = inO;

		for (int i = 19; i >= 0; --i) {
			ByteList newEncryptionKey;
			ByteList::iterator it = RC4Key.begin();
			for (; it != RC4Key.end(); ++it)
				newEncryptionKey.push_back((*it) ^ i);
			
			hashResult = RC4Encode(newEncryptionKey, hashResult);
		}
	}

	return algorithm3_6(inRevision,
						inLength,
						hashResult,
						inO,
						inP,
						inFileIDPart1,
						inEncryptMetaData,
						inU);
}

bool DecryptionHelper::AuthenticateUserPassword(const ByteList& inPassword) {
	return algorithm3_6(mRevision,
						mLength,
						inPassword,
						mO,
						mP,
						mFileIDPart1,
						mEncryptMetaData,
						mU);
}

bool DecryptionHelper::AuthenticateOwnerPassword(const ByteList& inPassword) {
	return algorithm3_7(mRevision,
						mLength,
						inPassword,
						mO,
						mP,
						mFileIDPart1,
						mEncryptMetaData,
						mU);
}


ByteList DecryptionHelper::stringToByteList(const std::string& inString) {
	ByteList buffer;
	std::string::const_iterator it = inString.begin();

	for (; it != inString.end(); ++it)
		buffer.push_back((Byte)*it);

	return buffer;
}
ByteList DecryptionHelper::substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength) {
	ByteList buffer;
	ByteList::const_iterator it = inList.begin();

	for (IOBasicTypes::LongBufferSizeType i = 0; i < inStart && it != inList.end(); ++i, ++it);

	for (IOBasicTypes::LongBufferSizeType i = 0; i < inLength && it != inList.end(); ++i,++it) 
		buffer.push_back((Byte)*it);

	return buffer;
}

void DecryptionHelper::append(ByteList& ioTargetList, const ByteList& inSource) {
	ByteList::const_iterator it = inSource.begin();

	for (; it != inSource.end(); ++it)
		ioTargetList.push_back(*it);
}

ByteList DecryptionHelper::add(const ByteList& inA, const ByteList& inB) {
	ByteList buffer;

	append(buffer, inA);
	append(buffer, inB);

	return buffer;
}