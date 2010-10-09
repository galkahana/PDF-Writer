#pragma once
#include "AbstractWrittenFont.h"

class WrittenFontTrueType : public AbstractWrittenFont
{
public:
	WrittenFontTrueType(ObjectsContext* inObjectsContext);
	~WrittenFontTrueType(void);

	virtual EStatusCode WriteFontDefinition(FreeTypeFaceWrapper& inFontInfo);

private:
	virtual bool AddToANSIRepresentation(const wstring& inText,
						 const UIntList& inGlyphsList,
						 UShortList& outEncodedCharacters);

};
