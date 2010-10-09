#pragma once

#include "IOBasicTypes.h"
#include <utility>

using namespace std;

typedef pair<bool,IOBasicTypes::Byte> BoolAndByte;


class WinAnsiEncoding
{
public:
	WinAnsiEncoding(void);
	~WinAnsiEncoding(void);

	BoolAndByte Encode(wchar_t inUnicodeCharacter);
	const char* GetEncodedGlyphName(IOBasicTypes::Byte inEncodedCharacter);
};
