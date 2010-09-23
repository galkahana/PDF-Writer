#pragma once
#include "IFreeTypeFaceExtender.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H 

class FreeTypeType1Wrapper : public IFreeTypeFaceExtender
{
public:
	FreeTypeType1Wrapper(FT_Face inFace);  // NEVER EVER EVER PASS NULL!!!!1
	virtual ~FreeTypeType1Wrapper(void);

	virtual	double GetItalicAngle();
	virtual BoolAndFTShort GetCapHeight();
	virtual BoolAndFTShort GetxHeight();
	virtual FT_UShort GetStemV();
	virtual EFontStretch GetFontStretch();
	virtual FT_UShort GetFontWeight();
	virtual bool HasSerifs();
	virtual bool IsForceBold();

private:
	PS_FontInfoRec mPSFontInfo;
	PS_PrivateRec mPrivateInfo;
	bool mPSavailable;
	bool mPSPrivateAvailable;

};
