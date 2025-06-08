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

/*
* XCryptor is a class that manages encryption keys for objects in a PDF document.
* It represents either a crypt filter, or a default encryption mechanism, in earlier revisions.
*/

class XCryptor {
public:

	XCryptor(bool isUsingAES, const ByteList& inFileEncryptionKey);
	virtual ~XCryptor(void);

	bool GetIsUsingAES();
	const ByteList& GetFileEncryptionKey();

	// Call on object start, will recompute a key for the new object (returns, so you can use if makes sense)
	const ByteList& OnObjectStart(long long inObjectID, long long inGenerationNumber);
	// Call on object end, will pop the computed key for this object
	void OnObjectEnd();
	const ByteList& GetCurrentObjectKey(); // will return empty key if stack is empty
			

private:
	ByteListList mEncryptionKeysStack;
	bool mUsingAES;
	ByteList mFileEncryptionKey;
};