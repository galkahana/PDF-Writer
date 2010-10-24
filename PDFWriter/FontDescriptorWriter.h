#pragma once

#include "ObjectsBasicTypes.h"
#include "WrittenFontRepresentation.h"

#include <string>
#include <vector>
#include <utility>

using namespace std;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class FreeTypeFaceWrapper;
class ObjectsContext;
class IFontDescriptorHelper;

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
								IFontDescriptorHelper* inDescriptorHelper);

private:
	unsigned int CalculateFlags(FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
	bool IsSymbolic(FreeTypeFaceWrapper* inFontInfo,
					const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
};
