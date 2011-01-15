#pragma once
#include "AbstractWrittenFont.h"

class WrittenFontTrueType : public AbstractWrittenFont
{
public:
	WrittenFontTrueType(ObjectsContext* inObjectsContext);
	~WrittenFontTrueType(void);

	virtual EStatusCode WriteFontDefinition(FreeTypeFaceWrapper& inFontInfo);

private:
	virtual bool AddToANSIRepresentation(	const GlyphUnicodeMappingList& inGlyphsList,
											UShortList& outEncodedCharacters);

	virtual bool AddToANSIRepresentation(
											const GlyphUnicodeMappingListList& inGlyphsList,
											UShortListList& outEncodedCharacters);

};
