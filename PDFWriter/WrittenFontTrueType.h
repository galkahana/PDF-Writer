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

	virtual bool AddToANSIRepresentation(const WStringList& inText,
							const UIntListList& inGlyphsList,
							UShortListList& outEncodedCharacters);

};
