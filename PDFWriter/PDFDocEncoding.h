#pragma once

#include "IOBasicTypes.h"
#include <utility>

using namespace std;

typedef pair<bool,IOBasicTypes::Byte> BoolAndByte;

class PDFDocEncoding
{
public:
	PDFDocEncoding(void);
	~PDFDocEncoding(void);

	BoolAndByte Encode(wchar_t inUnicodeCharacter);
};
