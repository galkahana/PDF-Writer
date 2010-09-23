#pragma once
#include "IFreeTypeFaceExtender.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H 

typedef std::pair<bool,FT_UShort> BoolAndFTUShort;


class FreeTypeOpenTypeWrapper : public IFreeTypeFaceExtender
{
public:
	FreeTypeOpenTypeWrapper(FT_Face inFace); // NEVER EVER EVER PASS NULL!!!!1
	virtual ~FreeTypeOpenTypeWrapper(void);

	virtual	double GetItalicAngle();
	virtual BoolAndFTShort GetCapHeight();
	virtual BoolAndFTShort GetxHeight();
	virtual FT_UShort GetStemV();
	virtual EFontStretch GetFontStretch();
	virtual FT_UShort GetFontWeight();
	virtual bool HasSerifs();
	virtual	bool IsForceBold();


private:
	TT_Postscript* mPostScriptTable;
	TT_OS2* mOS2Table;
	TT_PCLT* mPCLTTable;
	FT_Face mFace;

	double SixTeenFloatToDouble(FT_Fixed inFixed16_16);
	BoolAndFTUShort StemVFromLowerLWidth();
	EFontStretch GetFontStretchForPCLTValue(FT_Char inWidthValue);
	FT_UShort GetFontWeightFromPCLTValue(FT_Char inWeightValue);
};
