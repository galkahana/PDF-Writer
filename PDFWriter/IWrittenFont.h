#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "GlyphUnicodeMapping.h"

#include <list>
#include <vector>

using namespace std;

typedef list<unsigned short> UShortList;
typedef list<UShortList> UShortListList;
typedef list<GlyphUnicodeMappingList> GlyphUnicodeMappingListList;

class FreeTypeFaceWrapper;

class IWrittenFont
{
public:
	virtual ~IWrittenFont(){}	

	/*
		recieve a list of glyphs (and reference text, which can be ignored),
		and return an encoded list of charactrs to represent them. Also return if each charachter is single byte or multi (dbl) byte.
		this chap simply states whether this is non CID or CID. last but not least - return the object ID for the font, so that 
		the using content can refer to it
	*/
	virtual void AppendGlyphs(const GlyphUnicodeMappingList& inGlyphsList,
							  UShortList& outEncodedCharacters,
							  bool& outEncodingIsMultiByte,
							  ObjectIDType &outFontObjectID) = 0;

	virtual void AppendGlyphs(const GlyphUnicodeMappingListList& inGlyphsList,
							  UShortListList& outEncodedCharacters,
							  bool& outEncodingIsMultiByte,
							  ObjectIDType &outFontObjectID) = 0;

	/*
		Write a font definition using the glyphs appended.
	*/
	virtual EStatusCode WriteFontDefinition(FreeTypeFaceWrapper& inFontInfo) = 0;

};