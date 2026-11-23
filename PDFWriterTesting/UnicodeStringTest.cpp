/*
   Source File : UnicodeStringTest.cpp


   Copyright 2011 Gal Kahana PDFWriter

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
#include "UnicodeString.h"
#include "EStatusCode.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// Length-from-literal so embedded null bytes survive.
#define UTF16_BYTES(literal) (literal), (sizeof(literal) - 1)

// ---- FromUTF16 ------------------------------------------------------------

// A BOM-only input is the encoding of an empty UTF-16 string (what
// UnicodeString().ToUTF16BE(true) produces). FromUTF16 strips the BOM and
// hands a zero length payload to FromUTF16BE / FromUTF16LE, which used to
// underflow the loop bound and read past the buffer.
struct FromUTF16Case
{
	const char* mLabel;
	const char* mBytes;
	size_t mBytesLen;
	EStatusCode mExpectedStatus;
	const char* mExpectedUTF8; // checked only when mExpectedStatus is eSuccess
};

static const FromUTF16Case scFromUTF16Cases[] = {
	{"BigEndianBomOnly_ReturnsEmptyString",    UTF16_BYTES("\xFE\xFF"), eSuccess, ""},
	{"LittleEndianBomOnly_ReturnsEmptyString", UTF16_BYTES("\xFF\xFE"), eSuccess, ""},
	{"BigEndianSingleCharacter_ReturnsCharacter",    UTF16_BYTES("\xFE\xFF\x20\xAC"), eSuccess, "\xE2\x82\xAC"},
	{"LittleEndianSingleCharacter_ReturnsCharacter", UTF16_BYTES("\xFF\xFE\xAC\x20"), eSuccess, "\xE2\x82\xAC"},
	{"BigEndianSurrogatePair_ReturnsSupplementaryCharacter",    UTF16_BYTES("\xFE\xFF\xD8\x3D\xDE\x00"), eSuccess, "\xF0\x9F\x98\x80"},
	{"LittleEndianSurrogatePair_ReturnsSupplementaryCharacter", UTF16_BYTES("\xFF\xFE\x3D\xD8\x00\xDE"), eSuccess, "\xF0\x9F\x98\x80"},
	{"BigEndianTrailingHighSurrogate_ReturnsFailure",    UTF16_BYTES("\xFE\xFF\xD8\x3D"), eFailure, ""},
	{"LittleEndianTrailingHighSurrogate_ReturnsFailure", UTF16_BYTES("\xFF\xFE\x3D\xD8"), eFailure, ""},
};

static bool ExpectAllFromUTF16Results(const char* inFunction, const FromUTF16Case* inCases, size_t inCount)
{
	for(size_t i = 0; i < inCount; ++i)
	{
		const FromUTF16Case& c = inCases[i];

		// Arrange
		UnicodeString unicodeString;

		// Act
		EStatusCode status = unicodeString.FromUTF16((const unsigned char*)c.mBytes, (unsigned long)c.mBytesLen);

		// Assert
		if(status != c.mExpectedStatus)
		{
			cout << "UnicodeStringTest [" << inFunction << "::" << c.mLabel << "]: expected status " << c.mExpectedStatus << ", got " << status << "\n";
			return false;
		}
		if(eSuccess != c.mExpectedStatus)
			continue;

		EStatusCodeAndString utf8 = unicodeString.ToUTF8();
		if(eSuccess != utf8.first)
		{
			cout << "UnicodeStringTest [" << inFunction << "::" << c.mLabel << "]: failed to convert result to UTF-8\n";
			return false;
		}
		if(utf8.second != c.mExpectedUTF8)
		{
			cout << "UnicodeStringTest [" << inFunction << "::" << c.mLabel << "]: expected " << string(c.mExpectedUTF8).size() << " UTF-8 bytes, got " << utf8.second.size() << "\n";
			return false;
		}
	}
	return true;
}

static bool RunFromUTF16Cases()
{
	return ExpectAllFromUTF16Results(
				"FromUTF16",
				scFromUTF16Cases,
				sizeof(scFromUTF16Cases) / sizeof(scFromUTF16Cases[0]));
}

int UnicodeStringTest(int argc, char* argv[])
{
	if(!RunFromUTF16Cases())
		return 1;

	return 0;
}
