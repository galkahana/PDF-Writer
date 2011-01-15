#pragma once

#include <vector>
#include <list>

using namespace std;

typedef vector<unsigned long> ULongVector;

struct GlyphUnicodeMapping
{
	GlyphUnicodeMapping(){}

	GlyphUnicodeMapping(unsigned short inGlyphCode,unsigned long inUnicodeValue)
	{
		mGlyphCode = inGlyphCode;
		mUnicodeValues.push_back(inUnicodeValue);
	}
	
	GlyphUnicodeMapping(unsigned short inGlyphCode,ULongVector inUnicodeValues)
	{
		mGlyphCode = inGlyphCode;
		mUnicodeValues = inUnicodeValues;
	}

	// and ordered list of the unicode values that this glyph represents
	ULongVector mUnicodeValues;

	// glyph code
	unsigned short mGlyphCode;	
};

typedef list<GlyphUnicodeMapping> GlyphUnicodeMappingList;