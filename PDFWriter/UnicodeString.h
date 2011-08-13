#pragma once

#include "EStatusCode.h"

#include <string>
#include <list>

using namespace std;

typedef pair<PDFHummus::EStatusCode,string> EStatusCodeAndString;
typedef list<unsigned long> ULongList;
typedef list<unsigned short> UShortList;
typedef pair<PDFHummus::EStatusCode,UShortList> EStatusCodeAndUShortList;

class UnicodeString
{
public:
	UnicodeString(void);
	UnicodeString(const UnicodeString& inOtherString);
	UnicodeString(const ULongList& inOtherList);
	~UnicodeString(void);

	UnicodeString& operator =(const UnicodeString& inOtherString);
	UnicodeString& operator =(const ULongList& inOtherList);

	bool operator==(const UnicodeString& inOtherString) const;

	PDFHummus::EStatusCode FromUTF8(const string& inString);
	EStatusCodeAndString ToUTF8() const;

	// convert from UTF16 string, requires BOM
	PDFHummus::EStatusCode FromUTF16(const string& inString);
	PDFHummus::EStatusCode FromUTF16(const unsigned char* inString, unsigned long inLength);

	// convert from UTF16BE, do not include BOM
	PDFHummus::EStatusCode FromUTF16BE(const string& inString);
	PDFHummus::EStatusCode FromUTF16BE(const unsigned char* inString, unsigned long inLength);

	// convert from UTF16LE do not include BOM
	PDFHummus::EStatusCode FromUTF16LE(const string& inString);
	PDFHummus::EStatusCode FromUTF16LE(const unsigned char* inString, unsigned long inLength);

	// convert from unsigned shorts, does not require BOM, assuming that byte ordering is according to OS
	PDFHummus::EStatusCode FromUTF16UShort(const unsigned short* inShorts, unsigned long inLength);

	// convert to UTF16 BE
	EStatusCodeAndString ToUTF16BE(bool inPrependWithBom) const;
	
	// convert to UTF16 LE
	EStatusCodeAndString ToUTF16LE(bool inPrependWithBom) const;

	// covnert to unsigned shorts. byte ordering according to OS. not placing BOM
	EStatusCodeAndUShortList ToUTF16UShort() const;

	const ULongList& GetUnicodeList() const;
	ULongList& GetUnicodeList();
private:
	ULongList mUnicodeCharacters;
};
