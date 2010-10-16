#pragma once

#include "WrittenFontRepresentation.h"

#include <utility>
#include <vector>

using namespace std;

class DictionaryContext;
class ObjectsContext;
class FreeTypeFaceWrapper;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class IFontDescriptorCharsetWriter
{
public:

	virtual void WriteCharSet(	DictionaryContext* inDescriptorContext,
								ObjectsContext* inObjectsContext,
								FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs) =0;
};