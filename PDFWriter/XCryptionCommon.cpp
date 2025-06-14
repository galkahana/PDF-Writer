/*
Source File : XCryptionCommon.cpp


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
#include "XCryptionCommon.h"
#include "RC4.h"
#include "MD5Generator.h"

#include <algorithm>
#include <stdint.h>

using namespace std;
using namespace IOBasicTypes;


#define STANDARD_PASSWORD_LENGTH 32

const Byte scPaddingFiller[STANDARD_PASSWORD_LENGTH] = { 
	0x28,0xBF,0x4E,0x5E,0x4E,0x75,0x8A,0x41,
	0x64,0x00,0x4E,0x56,0xFF,0xFA,0x01,0x08,
	0x2E,0x2E,0x00,0xB6,0xD0,0x68,0x3E,0x80,
	0x2F,0x0C,0xA9,0xFE,0x64,0x53,0x69,0x7A 
};

const Byte scFixedEnd[] = { 0xFF,0xFF,0xFF,0xFF };


static ByteList standardizePassword(const ByteList& inPassword) {
	if(inPassword.size() > STANDARD_PASSWORD_LENGTH) {
		return substr(inPassword, 0, STANDARD_PASSWORD_LENGTH);
	} else if (inPassword.size() < STANDARD_PASSWORD_LENGTH) {
		ByteList standardizedPassword = inPassword;
		for(int i = 0; i < STANDARD_PASSWORD_LENGTH - inPassword.size(); ++i) {
			standardizedPassword.push_back(scPaddingFiller[i]);
		}
		return standardizedPassword;
	} else {
		return inPassword;
	}
}

static ByteList rc4Encode(const ByteList& inKey, const ByteList& inToEncode) {
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

static ByteList rc4Encode(const ByteList& inKey, const Byte* inToEncode, size_t inLength) {
	RC4 rc4(inKey);
	ByteList target;
	Byte buffer;

	for (size_t i = 0; i < inLength; ++i) {
		buffer = rc4.DecodeNextByte((Byte)inToEncode[i]);
		target.push_back(buffer);
	}
	return target;
}

XCryptionCommon::XCryptionCommon(void)
{
}

XCryptionCommon::~XCryptionCommon(void)
{
}

ByteList XCryptionCommon::RetrieveFileEncryptionKey(
	const ByteList& inUserPassword,
	unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData
) {
	// Algorithm 3.2 creates an encryption key based on provided password and 
	// document stored encryption parameters.
	// This encryption can can be used as basis for encrypting or decrypting the document content.
	// It is also used as part of the 3.3-3.5 algorithms in order to create encrypted
	// versions of user and owner passwords to be stored in the document, for a later
	// authentication by a reader application.


	MD5Generator md5;
	ByteList standardPassword = standardizePassword(inUserPassword);
	uint32_t truncP = uint32_t(inP);
	Byte truncPBuffer[4];
	ByteList hashResult;


	md5.Accumulate(standardPassword);
	md5.Accumulate(inO);
	for (int i = 0; i < 4; ++i) {
		truncPBuffer[i] = truncP & 0xFF;
		truncP >>= 8;
	}
	md5.Accumulate(truncPBuffer, 4);
	md5.Accumulate(inFileIDPart1);

	if (inRevision >= 4 && !inEncryptMetaData)
		md5.Accumulate(scFixedEnd, 4);

	hashResult = md5.ToString();

	if (inRevision >= 3) {
		for (int i = 0; i < 50; ++i) {
			MD5Generator anotherMD5;
			anotherMD5.Accumulate(substr(hashResult, 0, inLength));
			hashResult = anotherMD5.ToString();

		}
	}

	return inRevision == 2 ? substr(hashResult, 0, 5) : substr(hashResult, 0, inLength);	
}

const Byte scAESSuffix[] = { 0x73, 0x41, 0x6C, 0x54 };
ByteList XCryptionCommon::RetrieveObjectEncryptionKey(
	const ByteList& inFileEncryptionKey,
	bool inIsUsingAES,
	ObjectIDType inObjectNumber, 
	unsigned long inGenerationNumber) {
	// Algorithm 3.1 
	// Generates an object level encryption key.
	// It is an md hash of the file encryption key, the object number, and the generation number.  For AES it also includes a "sAlT" suffix.

	MD5Generator md5;
	ByteList result = inFileEncryptionKey;
	Byte buffer;
	LongBufferSizeType outputKeyLength = std::min<size_t>(inFileEncryptionKey.size() + 5, 16U);

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

	return substr(md5.ToString(), 0, outputKeyLength);
}

bool XCryptionCommon::AuthenticateUserPassword(
	// parameters for generating a file encryption key with the password and based on the a "CreateUValue" call to generate the input password based "U" value
	const ByteList& inPassword,
	unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData,
	// the original document "U" value to compare against, and determine if 
	const ByteList& inU
) {
	// Algorithm 3.6
	// This algorithm is intended to authenticate the user password.
	// the principal is simple: attempt to recreate a "U" value with the password. then compare it to the original "U" value.
	// If the two match, the password is authenticated. otherwise - not.

	// Create "file encryption key" based on the password and other parameters.
	ByteList fileEncryptionKey = RetrieveFileEncryptionKey(
		inPassword,
		inRevision,
		inLength,
		inO,
		inP,
		inFileIDPart1,
		inEncryptMetaData
	);

	// Create the "U" value based on the file encryption key.
	ByteList uValue = CreateUValue(fileEncryptionKey, inRevision, inFileIDPart1);

	// Compare the created "U" value to the original "U" value.
	if (inRevision == 2) {
		// with revision 2 we compare all 32 bytes of the U values
		return uValue == inU;
	} else {
		// remember the "arbitrary padding" of 16 bytes at the end of U? natrually no need to compare them...so just compare the first 16 bytes
		return substr(uValue, 0, 16) == substr(inU, 0, 16);
	}
}

ByteList XCryptionCommon::CreateUValue(
	const ByteList& inFileEncryptionKey,
	unsigned int inRevision,
	const ByteList& inFileIDPart1) {
	if (inRevision == 2) {
		// algorithm 3.4
		return rc4Encode(inFileEncryptionKey, scPaddingFiller, STANDARD_PASSWORD_LENGTH);		
	} else {
		// algorithm 3.5
		MD5Generator md5;
		ByteList hashResult;

		md5.Accumulate(scPaddingFiller, STANDARD_PASSWORD_LENGTH);
		md5.Accumulate(inFileIDPart1);
		hashResult = md5.ToString();

		hashResult = rc4Encode(inFileEncryptionKey, hashResult);

		for (Byte i = 1; i <= 19; ++i) {
			ByteList newEncryptionKey;
			ByteList::const_iterator it = inFileEncryptionKey.begin();
			for (; it != inFileEncryptionKey.end(); ++it)
				newEncryptionKey.push_back((*it) ^ i);
			hashResult = rc4Encode(newEncryptionKey, hashResult);
		}

		// as arbitrary padding, we append the first 16 bytes of the scPaddingFiller
		for(int i = 0; i < 16; ++i) {
			hashResult.push_back(scPaddingFiller[i]);
		}
		return hashResult;
	}
}

static ByteList createOwnerEncryptionKey(
	const ByteList& inOwnerPassword,
	unsigned int inRevision,
	unsigned int inLength) {
	// (a) to (d) of algirhtm 3.3 to create owner encryption key

	ByteList standardOwnerPassword = standardizePassword(inOwnerPassword);
	MD5Generator md5;
	ByteList hashResult;

	md5.Accumulate(standardOwnerPassword);

	hashResult = md5.ToString();

	if (inRevision >= 3) {
		for (int i = 0; i < 50; ++i) {
			MD5Generator anotherMD5;
			anotherMD5.Accumulate(hashResult);
			hashResult = anotherMD5.ToString();
		}
	}

	return substr(hashResult, 0, inRevision == 2 ? 5 : inLength);

}

ByteList XCryptionCommon::CreateOValue(	
	unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inOwnerPassword,
	const ByteList& inUserPassword) {
	// Algorithm 3.3 creates the standard security handler "O" value,
	// which is the owner key, allowing a reader to authenticate an owner password.
	// The process essentially creates an encryption key from the owner password and uses it to encrypt the user password.
	// The reverse process at 3.7, used to authenticate and input owner password, does the same process with the input, alleged owner password.

	// Part 1, prepare encryption key from owner password
	ByteList RC4Key = createOwnerEncryptionKey(
		inOwnerPassword,
		inRevision,
		inLength
	);

	// Part 2, encrypt user password with the encryption key
	ByteList standardUserPassword = standardizePassword(inUserPassword);
	ByteList encryptedPassword = rc4Encode(RC4Key, standardUserPassword);

	if (inRevision >= 3) {
		for (Byte i = 1; i <= 19; ++i) {
			ByteList newRC4Key;
			ByteList::iterator it = RC4Key.begin();
			for (; it != RC4Key.end(); ++it)
				newRC4Key.push_back((*it) ^ i);
			encryptedPassword = rc4Encode(newRC4Key, encryptedPassword);
		}
	}

	return encryptedPassword;
}

bool XCryptionCommon::AuthenticateOwnerPassword(
	const ByteList& inMaybeOwnerPassword,
	unsigned int inRevision,
	unsigned int inLength,
	const ByteList& inO,
	long long inP,
	const ByteList& inFileIDPart1,
	bool inEncryptMetaData,
	const ByteList inU) {
	// algorithm 3.7 is intended to authenticate the owner passowrd.
	// It converts the owner password to encryption key similarly to algorithm 3.3, and then uses that key to decrypt
	// a "user password" from the O value. now just authenticate the result algorithm 3.6 as one would with a user.
	// if that "user password" is authenaticated, this means teh owner password is authenticated as well.

	// Part 1: Create what is maybe the owner encryption key based on the candidate owner password.
	ByteList RC4Key = createOwnerEncryptionKey(
		inMaybeOwnerPassword,
		inRevision,
		inLength
	);

	ByteList decryptedUserPassword;

	// Part 2: Decrypt the "user password" from the O value using the maybe owner encryption key.
	// This is the "reverse" application of similar steps in algorithm 3.3, which created the "O" value.
	if (inRevision == 2) {
		decryptedUserPassword = rc4Encode(RC4Key, inO);
	}
	else if (inRevision >= 3) {
		decryptedUserPassword = inO;

		for (int i = 19; i >= 0; --i) {
			ByteList newEncryptionKey;
			ByteList::iterator it = RC4Key.begin();
			for (; it != RC4Key.end(); ++it)
				newEncryptionKey.push_back((*it) ^ i);

			decryptedUserPassword = rc4Encode(newEncryptionKey, decryptedUserPassword);
		}
	}

	// Part 3: attempt to authenticate the user password, using the decrypted "user password" as the input password.
	return AuthenticateUserPassword(
		decryptedUserPassword,
		inRevision,
		inLength,
		inO,
		inP,
		inFileIDPart1,
		inEncryptMetaData,
		inU
	);
}
