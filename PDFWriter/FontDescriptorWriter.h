#pragma once

#include "ObjectsBasicTypes.h"
#include "WrittenFontRepresentation.h"

#include <string>
#include <vector>
#include <utility>

using namespace std;


using namespace std;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class FreeTypeFaceWrapper;
class ObjectsContext;
class IFontDescriptorCharsetWriter;

class FontDescriptorWriter
{
public:
	FontDescriptorWriter(void);
	~FontDescriptorWriter(void);

	void WriteFontDescriptor(	ObjectIDType inFontDescriptorObjectID,
								const string& inFontPostscriptName,
								FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
								ObjectsContext* inObjectsContext,
								IFontDescriptorCharsetWriter* inCharsetWriter);

private:
	unsigned int CalculateFlags(FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
	bool IsSymbolic(FreeTypeFaceWrapper* inFontInfo,
					const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
};
