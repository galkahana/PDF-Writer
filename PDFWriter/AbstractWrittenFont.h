#pragma once

#include "IWrittenFont.h"
#include "WrittenFontRepresentation.h"

class ObjectsContext;

class AbstractWrittenFont : public IWrittenFont
{
public:
	AbstractWrittenFont(ObjectsContext* inObjectsContext);
	virtual ~AbstractWrittenFont(void);

	virtual void AppendGlyphs(const UIntList& inGlyphsList,
							  const wstring& inText,
							  UShortList& outEncodedCharacters,
							  bool& outEncodingIsMultiByte,
							  ObjectIDType &outFontObjectID);

protected:
	WrittenFontRepresentation* mCIDRepresentation;
	WrittenFontRepresentation* mANSIRepresentation;
	ObjectsContext* mObjectsContext;

private:
	bool CanEncodeWithIncludedChars(WrittenFontRepresentation* inRepresentation, 
									const UIntList& inGlyphsList,
									UShortList& outEncodedCharacters);
	void AddToCIDRepresentation(const std::wstring& inText,const UIntList& inGlyphsList,UShortList& outEncodedCharacters);
	
	// Aha! This method remains virtual for sub implementations to 
	// override. Adding to an ANSI representation is dependent on the output format,
	// where True Type has some different ruling from OpenType(CFF)/Type1
	virtual bool AddToANSIRepresentation(const wstring& inText,
								 const UIntList& inGlyphsList,
								 UShortList& outEncodedCharacters) = 0;
};
