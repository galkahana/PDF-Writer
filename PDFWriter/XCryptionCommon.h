/*
Source File : XCryptionCommon.h


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

#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"

#include <list>
#include <string>
#include <map>

class XCryptionCommon;

typedef std::list<IOBasicTypes::Byte> ByteList;
typedef std::list<ByteList> ByteListList;
typedef std::map<std::string, XCryptionCommon*> StringToXCryptionCommonMap;


class XCryptionCommon {
public:

	XCryptionCommon(void);
	virtual ~XCryptionCommon(void);

	/*
		To start using XCryptionCommon do this:

		1. Create an instance of the object
		2. Call Initial Setup with the PDF level. return bool will tell you if XCryption is supported for this input. if not...don't bother with the rest
		3. Call SetupInitialEncryptionKey. This may require some initial computations with other algorithms of this object, per what input you may possess. It is fine
			to call any method prior to SetupInitialEncryptionKey, but those that relate to state - OnObjectStart,OnObjectEnd, GetCurrentObjectKey. these require the computation.
			Call this whenever you acquired the required input for it, no matter when. 
		4. now you can call all methods freely
	*/

	// call this whenever you first can. 
	void Setup(bool inUsingAES);

	void SetupInitialEncryptionKey(const std::string& inUserPassword,
		unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData
		); // with alrogithm 3.2
	// Setup key directly
	void SetupInitialEncryptionKey(const ByteList& inEncryptionKey);


	// Call on object start, will recompute a key for the new object (returns, so you can use if makes sense)
	const ByteList& OnObjectStart(long long inObjectID, long long inGenerationNumber);
	// Call on object end, will pop the computed key for this object
	void OnObjectEnd();
	const ByteList& GetCurrentObjectKey(); // will return empty key if stack is empty

	// get original encryption key (without particular object additions)
	const ByteList& GetInitialEncryptionKey() const;

	// bytelist operations (class methods)
	static ByteList stringToByteList(const std::string& inString);
	static ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inEnd);
	static void append(ByteList& ioTargetList, const ByteList& inSource);
	static ByteList concat(const ByteList& inA, const ByteList& inB);
	static std::string ByteListToString(const ByteList& inByteList);

	// PDF xcryption algorithms (Important! length must be passed in bytes and not in bits. which normally means - the PDF value / 8)
	
	// Algorithm 3.1 is general (not security handlers specific). 
	// It creates an ecnryption key for aes/rc4 based on security handler document level 
	// encryption key, object number and generation number
	ByteList algorithm3_1(ObjectIDType inObjectNumber,
		unsigned long inGenerationNumber,
		const ByteList& inEncryptionKey,
		bool inIsUsingAES);
	
	// Starting from algorithm 3.2 the algorithms are specific to standard security handler
	// standard security handlers allows for the user access permisions, as well as of user and owner passwords (owner password 
	// permits full control, while user passowrd provide only the control defined by the defined access permissions).


	// Algorithm 3.2 creates an encryption key based on provided password and 
	// document stored encryption parameters.
	// This encryption can can be used as basis for encrypting or decrypting the document content.
	// It is also used as part of the 3.3-3.5 algorithms in order to create encrypted
	// versions of user and owner passwords to be stored in the document, for a later
	// authentication by a reader application.
	ByteList algorithm3_2(unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inPassword,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData);

	// Algorithm 3.3 creates the standard security handler "O" value,
	// which is the owner key, allowing a reader to authenticate an owner password.
	// The process essentially creates an encryption key from the owner password and uses it to encrypt the user password.
	// The reverse process at 3.7, used to authenticate and input owner password, does the same process with the input, alleged owner password.
	ByteList algorithm3_3(unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inOwnerPassword,
		const ByteList& inUserPassword);

	// algorithm 3.4 and 3.5 create the standard security handler "U" value,
	// which is the user key, allowing a reader to authenticate a user password.
	// 3_4 is used when the revision 2, while 3_5 is for revision 3 and above.
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

	// algorithm 3.6 is intended to authenticate the user passowrd. 
	// It essentially runs 3.4 or 3.5 (based on revision) and compares the result
	// to the provided oU value.
	bool algorithm3_6(unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inPassword,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData,
		const ByteList inU);
	// algorithm 3.7 is intended to authenticate the owner passowrd.
	// It converts the owner password to a user password with the same algorithm as 3.3, and then
	// continues to authenticate the resultant user password with algorithm 3.6.
	bool algorithm3_7(unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inPassword,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData,
		const ByteList inU);

	bool IsUsingAES();

private:
	ByteList mPaddingFiller;
	ByteListList mEncryptionKeysStack;
	bool mUsingAES;
	ByteList mEncryptionKey;

	ByteList RC4Encode(const ByteList& inKey, const ByteList& inToEncode);
	ByteList ComputeEncryptionKeyForObject(ObjectIDType inObjectNumber, unsigned long inGenerationNumber); // with algorithm3_1

};