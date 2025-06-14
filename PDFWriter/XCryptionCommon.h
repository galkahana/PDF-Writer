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

#include "ObjectsBasicTypes.h"
#include "ByteList.h"

#include <list>

/**
 * XCryption common implements encryption and decryption functionality for PDF files pre PDF 2.0.
 * It fits encryption dicts where the "V" is either 1, 2 or 4. 
 * In PDF 2.0, those algorithms are decpreated and XCryptionCommon2_0 should be used instead.
 * 
 */
class XCryptionCommon {
public:

	XCryptionCommon(void);
	virtual ~XCryptionCommon(void);


	ByteList RetrieveFileEncryptionKey(
		const ByteList& inUserPassword,
		unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData
    );

	ByteList RetrieveObjectEncryptionKey(
		const ByteList& inFileEncryptionKey,
		bool inIsUsingAES,
		ObjectIDType inObjectNumber, 
		unsigned long inGenerationNumber);

    bool AuthenticateUserPassword(
		const ByteList& inMaybeUserPassword,
		unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData,

		// the original document "U" value to compare against, and determine if
		// the password is authenticated
		const ByteList& inU
    );

	bool AuthenticateOwnerPassword(
		const ByteList& inMaybeOwnerPassword,
		unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inO,
		long long inP,
		const ByteList& inFileIDPart1,
		bool inEncryptMetaData,

		// the original document "U" value to compare against, and determine if
		// the password is authenticated
		const ByteList inU		
	);

	ByteList CreateUValue(
		const ByteList& inFileEncryptionKey,
		unsigned int inRevision,
		const ByteList& inFileIDPart1);

	ByteList CreateOValue(
		unsigned int inRevision,
		unsigned int inLength,
		const ByteList& inOwnerPassword,
		const ByteList& inUserPassword
	);

};