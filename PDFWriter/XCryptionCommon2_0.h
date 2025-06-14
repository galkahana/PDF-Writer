/*
   Source File : XCryptionCommon2_0.h


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

#ifndef PDFHUMMUS_NO_OPENSSL
#include "ByteList.h"

typedef std::pair<ByteList, ByteList> ByteListPair;

/*
 * XCryption common implements encryption and decryption functionality for PDF files of version 2.0 (or higher?!).
 * It fits encryption dicts where the "V" is either 5 (and standard handler revision 6). 
 * For pre PDF 2.0, XCryptionCommon should be used instead.
 */
class XCryptionCommon2_0 {
public:
    XCryptionCommon2_0(void);
    virtual ~XCryptionCommon2_0(void);


    // 1. For when reading
    ByteList RetrieveFileEncryptionKey(
        const ByteList& inPassword, // user or owner password
        const ByteList& inO,
        const ByteList& inU,
        const ByteList& inOE,
        const ByteList& inUE
    );
    bool AuthenticateUserPassword(
        const ByteList& inPassword,
        const ByteList& inU
    );
    bool AuthenticateOwnerPassword(
        const ByteList& inPassword,
        const ByteList& inO,
        const ByteList& inU
    );
    bool AuthenticaePerms(
        const ByteList& inFileEncryptionKey,
        const ByteList& inPerms,
        long long inP
    );

    // 2. for when writing
    ByteList GenerateFileEncryptionKey();
    ByteListPair CreateUandUEValues(
        const ByteList& inUserPassword,
        const ByteList& inFileEncryptionKey
    );
    ByteListPair CreateOandOEValues(
        const ByteList& inOwnerPassword,
        const ByteList& inFileEncryptionKey,
        const ByteList& inU
    );
    ByteList CreatePerms(
        const ByteList& inFileEncryptionKey,
        long long inP,
        bool inEncryptMetadata
    );
};

#endif // PDFHUMMUS_NO_OPENSSL