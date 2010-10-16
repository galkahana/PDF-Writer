#pragma once
#include "EStatusCode.h"
#include "WrittenFontRepresentation.h"
#include "ObjectsBasicTypes.h"
#include "IFontDescriptorCharsetWriter.h"
#include "IDescendentFontWriter.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <list>

class FreeTypeFaceWrapper;
class ObjectsContext;
class DictionaryContext;


using namespace std;

typedef list<FT_Pos> FTPosList;

class DescendentFontWriter : public IFontDescriptorCharsetWriter
{
public:
	DescendentFontWriter(void);
	~DescendentFontWriter(void);

	// the IDescendentFontWriter input pointer will implement the font specific differences
	virtual EStatusCode WriteFont(	ObjectIDType inDecendentObjectID, 
									const string& inFontName,
									FreeTypeFaceWrapper& inFontInfo,
									const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
									ObjectsContext* inObjectsContext,
									IDescendentFontWriter* inDescendentFontWriterHelper);

	// IFontDescriptorCharsetWriter implementation [would probably evolve at some point to IDescriptorWriterHelper...
	virtual void WriteCharSet(	DictionaryContext* inDescriptorContext,
								ObjectsContext* inObjectsContext,
								FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
private:

	FreeTypeFaceWrapper* mFontInfo;
	ObjectsContext* mObjectsContext;
	ObjectIDType mCIDSetObjectID;


	void WriteWidths(	const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
						DictionaryContext* inFontContext);
	void WriteCIDSystemInfo(ObjectIDType inCIDSystemInfoObjectID);
	void WriteWidthsItem(bool inAllWidthsSame,const FTPosList& inWidths,unsigned short inFirstCID, unsigned short inLastCID);
	void WriteCIDSet(const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
};
