#pragma once
#include "AbstractWrittenFont.h"

class WrittenFontTrueType : public AbstractWrittenFont
{
public:
	WrittenFontTrueType(ObjectsContext* inObjectsContext);
	~WrittenFontTrueType(void);

private:
	virtual bool AddToANSIRepresentation(const wstring& inText,
						 const UIntList& inGlyphsList,
						 UShortList& outEncodedCharacters);

};
