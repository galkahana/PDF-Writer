#pragma once

#include "IWrittenFont.h"
#include "WrittenFontRepresentation.h"
#include "GlyphUnicodeMapping.h"

class ObjectsContext;

class AbstractWrittenFont : public IWrittenFont
{
public:
	AbstractWrittenFont(ObjectsContext* inObjectsContext);
	virtual ~AbstractWrittenFont(void);

	virtual void AppendGlyphs(const GlyphUnicodeMappingList& inGlyphsList,
							  UShortList& outEncodedCharacters,
							  bool& outEncodingIsMultiByte,
							  ObjectIDType &outFontObjectID);
	virtual void AppendGlyphs(const GlyphUnicodeMappingListList& inGlyphsList,
							  UShortListList& outEncodedCharacters,
							  bool& outEncodingIsMultiByte,
							  ObjectIDType &outFontObjectID);
protected:
	WrittenFontRepresentation* mCIDRepresentation;
	WrittenFontRepresentation* mANSIRepresentation;
	ObjectsContext* mObjectsContext;

private:
	bool CanEncodeWithIncludedChars(WrittenFontRepresentation* inRepresentation, 
									const GlyphUnicodeMappingList& inGlyphsList,
									UShortList& outEncodedCharacters);
	bool CanEncodeWithIncludedChars(WrittenFontRepresentation* inRepresentation, 
									const GlyphUnicodeMappingListList& inGlyphsList,
									UShortListList& outEncodedCharacters);

	void AddToCIDRepresentation(const GlyphUnicodeMappingList& inGlyphsList,UShortList& outEncodedCharacters);
	void AddToCIDRepresentation(const GlyphUnicodeMappingListList& inGlyphsList,UShortListList& outEncodedCharacters);
	
	// Aha! This method remains virtual for sub implementations to 
	// override. Adding to an ANSI representation is dependent on the output format,
	// where True Type has some different ruling from OpenType(CFF)/Type1
	virtual bool AddToANSIRepresentation(
									const GlyphUnicodeMappingList& inGlyphsList,
									UShortList& outEncodedCharacters) = 0;
	virtual bool AddToANSIRepresentation(
									const GlyphUnicodeMappingListList& inGlyphsList,
									UShortListList& outEncodedCharacters) = 0;
};
