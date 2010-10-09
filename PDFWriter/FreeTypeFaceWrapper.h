#pragma once

#include "EFontStretch.h"
#include "EStatusCode.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <utility>
#include <list>

class IFreeTypeFaceExtender;
class IWrittenFont;
class ObjectsContext;

using namespace std;

typedef pair<bool,FT_Short> BoolAndFTShort;
typedef list<unsigned int> UIntList;

class FreeTypeFaceWrapper
{
public:
	FreeTypeFaceWrapper(FT_Face inFace);
	~FreeTypeFaceWrapper(void);

	FT_Error DoneFace();

	FT_Face operator->();
	operator FT_Face();

	bool IsValid();

	EStatusCode GetGlyphsForUnicodeText(const wstring& inText,UIntList& outGlyphs);

	double GetItalicAngle();
	BoolAndFTShort GetCapHeight();
	BoolAndFTShort GetxHeight();
	FT_UShort GetStemV();
	EFontStretch GetFontStretch();
	FT_UShort GetFontWeight();
	// these would be flags for the font as a whole. if subsetting, match to the character set
	unsigned int GetFontFlags();

	// Create the written font object, matching to write this font in the best way.
	IWrittenFont* CreateWrittenFontObject(ObjectsContext* inObjectsContext);


	// flags determining values
	bool IsFixedPitch();
	bool IsSerif();
	bool IsScript();
	bool IsItalic();
	bool IsForceBold();

	// will be used externally to determine if font is symbolic or not
	bool IsCharachterCodeAdobeStandard(FT_ULong inCharacterCode);

private:

	FT_Face mFace;
	IFreeTypeFaceExtender* mFormatParticularWrapper;
	bool mHaslowercase;

	void SetupFormatSpecificExtender();
	BoolAndFTShort CapHeightFromHHeight();
	BoolAndFTShort XHeightFromLowerXHeight();
	BoolAndFTShort GetYBearingForUnicodeChar(unsigned short unicodeCharCode);
	EFontStretch StretchFromName();
	FT_UShort WeightFromName();
	bool IsSymbolic();
	bool IsDefiningCharsNotInAdobeStandardLatin();
};
