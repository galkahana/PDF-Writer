#pragma once
#include "IFreeTypeFaceExtender.h"
#include "PFMFileReader.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H 

class FreeTypeType1Wrapper : public IFreeTypeFaceExtender
{
public:
	FreeTypeType1Wrapper(FT_Face inFace,const wstring& inPFMFilePath);  // NEVER EVER EVER PASS NULL!!!!1 [ok to pass empty string for PFM file]
	virtual ~FreeTypeType1Wrapper(void);

	virtual	double GetItalicAngle();
	virtual BoolAndFTShort GetCapHeight();
	virtual BoolAndFTShort GetxHeight();
	virtual FT_UShort GetStemV();
	virtual EFontStretch GetFontStretch();
	virtual FT_UShort GetFontWeight();
	virtual bool HasSerifs();
	virtual bool IsScript();
	virtual bool IsForceBold();

private:
	bool mPFMFileInfoRelevant;
	PFMFileReader mPFMReader;
	PS_FontInfoRec mPSFontInfo;
	PS_PrivateRec mPrivateInfo;
	bool mPSavailable;
	bool mPSPrivateAvailable;

};
