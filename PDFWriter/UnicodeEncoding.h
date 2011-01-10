#pragma once

#include "EStatusCode.h"
#include <string>
#include <vector>

using namespace std;

typedef vector<unsigned long> ULongVector;

struct UTF16Encoding
{
	unsigned short HighSurrogate;
	unsigned short LowSurrogate;
};

class UnicodeEncoding
{
public:
	UnicodeEncoding(void);
	~UnicodeEncoding(void);

	EStatusCode UTF16ToUnicode(const wstring& inText,ULongVector& outUnicodeIndexes);

	bool IsSupplementary(unsigned long inUnicodeCharacter);
	UTF16Encoding EncodeCharater(unsigned long inUnicodeCharacter);

};
