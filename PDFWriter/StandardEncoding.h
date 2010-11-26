#pragma once
#include "IOBasicTypes.h"


class StandardEncoding
{
public:
	StandardEncoding(void);
	~StandardEncoding(void);

	const char* GetEncodedGlyphName(IOBasicTypes::Byte inEncodedCharacter);
};
