#pragma once

#include "EFontStretch.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <utility>

class IFreeTypeFaceExtender;

typedef std::pair<bool,FT_Short> BoolAndFTShort;

class FreeTypeFaceWrapper
{
public:
	FreeTypeFaceWrapper(FT_Face inFace);
	~FreeTypeFaceWrapper(void);

	FT_Error DoneFace();

	FT_Face operator->();
	operator FT_Face();

	bool IsValid();

	// implementation dependent values
	double GetItalicAngle();
	BoolAndFTShort GetCapHeight();
	BoolAndFTShort GetxHeight();
	FT_UShort GetStemV();
	EFontStretch GetFontStretch();
	FT_UShort GetFontWeight();
	unsigned int GetFontFlags();



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
	bool IsFixedPitch();
	bool IsSerif();
	bool IsSymbolic();
	bool IsDefiningCharsNotInAdobeStandardLatin();
	bool IsCharachterCodeAdobeStandard(FT_ULong inCharacterCode);
	bool IsScript();
	bool IsItalic();
	bool IsForceBold();
};
