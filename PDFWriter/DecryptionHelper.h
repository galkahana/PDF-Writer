/*
Source File : DecryptionHelper.h


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
#pragma once

#include "EStatusCode.h"
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"

#include <list>
#include <string>

class PDFParser;
class IByteReader;
class PDFStreamInput;
class PDFObject;

typedef std::list<IOBasicTypes::Byte> ByteList;
typedef std::list<ByteList> ByteListList;

class DecryptionHelper {

public:
	DecryptionHelper(void);
	virtual ~DecryptionHelper(void);


	/*
		Setup will fail for case of bad password, for decryption that can be handled.
	*/
	PDFHummus::EStatusCode Setup(PDFParser* inParser,const std::string& inPassword);

	/*
		Queries.

		1. IsEncrypted means PDF is encrypted
		2. SupportsDecryption means that DecryptionHelper supports the kind of encryption defined in the PDF
		3. CanDecryptDocument means that DecryptionHelper can decrypt document. means decryption is supported, and password is good
		4. DidFailPasswordVerification meams that password verification didnt went through. either decryption not supported, or password no good
		5. DidSucceedOwnerPasswordVerification means that password verification succeeded, and the user has owner priviliges
	*/
	bool IsEncrypted();
	bool SupportsDecryption();
	bool CanDecryptDocument();
	bool DidFailPasswordVerification();
	bool DidSucceedOwnerPasswordVerification();

	std::string DecryptString(const std::string& inStringToDecrypt);

	void OnObjectStart(long long inObjectID, long long inGenerationNumber);
	void OnObjectEnd(PDFObject* inObject);
	IByteReader* CreateDecryptionFilterForStream(PDFStreamInput* inStream, IByteReader* inToWrapStream);


private:

	PDFParser* mParser;
	bool mIsEncrypted;
	bool mSupportsDecryption;
	ByteListList mEncryptionKeysStack;
	bool mUsingAES;
	ByteList mPaddingFiller;
	ByteList mEncryptionKey;

	ByteList RC4Encode(const ByteList& inKey, const ByteList& inToEncode);

	ByteList stringToByteList(const std::string& inString);
	ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inEnd);
	void append(ByteList& ioTargetList, const ByteList& inSource);
	ByteList add(const ByteList& inA, const ByteList& inB);


	// Generic encryption
	unsigned int mV;
	unsigned int mLength; // mLength is in bytes!
	
	IByteReader* CreateDecryptionReader(IByteReader* inSourceStream,const ByteList& inEncryptionKey);
	ByteList ComputeEncryptionKeyForObject(ObjectIDType inObjectNumber, unsigned long inGenerationNumber); // with algorithm3_1
	ByteList algorithm3_1(ObjectIDType inObjectNumber,
							unsigned long inGenerationNumber,
							const ByteList& inEncryptionKey,
							bool inIsUsingAES);

	// Standard filter specific
	bool mFailedPasswordVerification;
	bool mDidSucceedOwnerPasswordVerification;

	unsigned int mRevision;
	ByteList mO;
	ByteList mU;
	long long mP;
	bool mEncryptMetaData;
	ByteList mFileIDPart1;

	ByteList ComputeEncryptionKey(const ByteList& inPassword); // with algorithm3_2
	bool AuthenticateUserPassword(const ByteList& inPassword);
	bool AuthenticateOwnerPassword(const ByteList& inPassword);
	ByteList algorithm3_2(unsigned int inRevision,
							 unsigned int inLength,
							 const ByteList& inPassword,
							 const ByteList& inO,
							 long long inP,
							 const ByteList& inFileIDPart1,
							 bool inEncryptMetaData);
	ByteList algorithm3_3(unsigned int inRevision,
							unsigned int inLength,
							const ByteList& inOwnerPassword,
							const ByteList& inUserPassword);
	ByteList algorithm3_4(unsigned int inLength,
							const ByteList& inUserPassword,
							const ByteList& inO,
							long long inP,
							const ByteList& inFileIDPart1,
							bool inEncryptMetaData);
	ByteList algorithm3_5(unsigned int inRevision,
								unsigned int inLength,
								const ByteList& inUserPassword,
								const ByteList& inO,
								long long inP,
								const ByteList& inFileIDPart1,
								bool inEncryptMetaData);
	bool algorithm3_6(unsigned int inRevision,
						unsigned int inLength,
						const ByteList& inPassword,
						const ByteList& inO,
						long long inP,
						const ByteList& inFileIDPart1,
						bool inEncryptMetaData,
						const ByteList inU);
	bool algorithm3_7(unsigned int inRevision,
					unsigned int inLength,
					const ByteList& inPassword,
					const ByteList& inO,
					long long inP,
					const ByteList& inFileIDPart1,
					bool inEncryptMetaData,
					const ByteList inU);
};