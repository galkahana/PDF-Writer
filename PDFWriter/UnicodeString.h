#pragma once

#include "EPDFStatusCode.h"

#include <string>
#include <list>

using namespace std;

typedef pair<EPDFStatusCode,string> EPDFStatusCodeAndString;
typedef list<unsigned long> ULongList;
typedef list<unsigned short> UShortList;
typedef pair<EPDFStatusCode,UShortList> EPDFStatusCodeAndUShortList;

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

	EPDFStatusCode FromUTF8(const string& inString);
	EPDFStatusCodeAndString ToUTF8() const;

	// convert from UTF16 string, requires BOM
	EPDFStatusCode FromUTF16(const string& inString);
	EPDFStatusCode FromUTF16(const unsigned char* inString, unsigned long inLength);

	// convert from UTF16BE, do not include BOM
	EPDFStatusCode FromUTF16BE(const string& inString);
	EPDFStatusCode FromUTF16BE(const unsigned char* inString, unsigned long inLength);

	// convert from UTF16LE do not include BOM
	EPDFStatusCode FromUTF16LE(const string& inString);
	EPDFStatusCode FromUTF16LE(const unsigned char* inString, unsigned long inLength);

	// convert from unsigned shorts, does not require BOM, assuming that byte ordering is according to OS
	EPDFStatusCode FromUTF16UShort(unsigned short* inShorts, unsigned long inLength);

	// convert to UTF16 BE
	EPDFStatusCodeAndString ToUTF16BE(bool inPrependWithBom) const;
	
	// convert to UTF16 LE
	EPDFStatusCodeAndString ToUTF16LE(bool inPrependWithBom) const;

	// covnert to unsigned shorts. byte ordering according to OS. not placing BOM
	EPDFStatusCodeAndUShortList ToUTF16UShort() const;

	const ULongList& GetUnicodeList() const;
	ULongList& GetUnicodeList();
private:
	ULongList mUnicodeCharacters;
};
