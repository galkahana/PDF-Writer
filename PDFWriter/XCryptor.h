/*
Source File : XCryptor.h


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
#pragma once

#include "ObjectsBasicTypes.h"
#include "ByteList.h"

#include <list>
#include <string>
#include <map>

class XCryptor;

typedef std::list<ByteList> ByteListList;
typedef std::map<std::string, XCryptor*> StringToXCryptorMap;


enum EXCryptorAlgo {
	eRC4 = 0,
	eAESV2 = 1,
	eAESV3 = 2
};

/*
* XCryptor is a class that manages encryption keys for objects in a PDF document.
* It represents either a crypt filter, or a default encryption mechanism, in earlier revisions.
*/

class XCryptor {
public:

	XCryptor(EXCryptorAlgo inExcryptorAlgo, const ByteList& inFileEncryptionKey);
	virtual ~XCryptor(void);

	EXCryptorAlgo GetExcryptorAlgo() const;
	// returns true if xcryptor algo is AES, false if RC4
	bool GetIsUsingAES() const;
	const ByteList& GetFileEncryptionKey() const;

	// Call on object start, will recompute a key for the new object (returns, so you can use if makes sense)
	const ByteList& OnObjectStart(long long inObjectID, long long inGenerationNumber);
	// Call on object end, will pop the computed key for this object
	void OnObjectEnd();
	const ByteList& GetCurrentObjectKey() const; // will return empty key if stack is empty
			

private:
	ByteListList mEncryptionKeysStack;
	EXCryptorAlgo mExcryptorAlgo;
	ByteList mFileEncryptionKey;


	bool GetIsUsing2_0() const; 
};