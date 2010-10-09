#pragma once

#include "ObjectsBasicTypes.h"

#include <map>

using namespace std;


struct GlyphEncodingInfo
{
	unsigned short mEncodedCharacter;
	unsigned short mUnicodeCharacter;

	GlyphEncodingInfo(unsigned short inEncodedCharacter,unsigned short inUnicodeCharacter)
	{mEncodedCharacter = inEncodedCharacter; mUnicodeCharacter = inUnicodeCharacter;}
};

typedef map<unsigned int, GlyphEncodingInfo> UIntToGlyphEncodingInfoMap;

struct WrittenFontRepresentation
{	
	WrittenFontRepresentation(){mWrittenObjectID = 0;}

	UIntToGlyphEncodingInfoMap mGlyphIDToEncodedChar;
	ObjectIDType mWrittenObjectID;
};
