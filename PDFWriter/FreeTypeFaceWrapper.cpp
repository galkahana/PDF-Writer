/*
   Source File : FreeTypeFaceWrapper.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


*/
#include "FreeTypeFaceWrapper.h"
#include "IFreeTypeFaceExtender.h"
#include "FreeTypeType1Wrapper.h"
#include "FreeTypeOpenTypeWrapper.h"
#include "Trace.h"
#include "BetweenIncluding.h"
#include "WrittenFontCFF.h"
#include "WrittenFontTrueType.h"

#include FT_XFREE86_H
#include FT_CID_H

using namespace PDFHummus;

FreeTypeFaceWrapper::FreeTypeFaceWrapper(FT_Face inFace,const string& inFontFilePath,bool inDoOwn)
{
	mFace = inFace;
	mFontFilePath = inFontFilePath;
	SetupFormatSpecificExtender("");
	mDoesOwn = inDoOwn;
}

FreeTypeFaceWrapper::FreeTypeFaceWrapper(FT_Face inFace,const string& inFontFilePath,const string& inPFMFilePath, bool inDoOwn)
{
	mFace = inFace;
	mFontFilePath = inFontFilePath;
	string fileExtension = GetExtension(inPFMFilePath);
	if(fileExtension == "PFM" || fileExtension ==  "pfm") // just don't bother if it's not PFM
		SetupFormatSpecificExtender(inPFMFilePath);
	else
		SetupFormatSpecificExtender("");
	mDoesOwn = inDoOwn;
}

string FreeTypeFaceWrapper::GetExtension(const string& inFilePath)
{
	string::size_type dotPosition = inFilePath.rfind(".");

	if(inFilePath.npos == dotPosition || (inFilePath.size() - 1) == dotPosition)
		return "";
	else
		return inFilePath.substr(dotPosition + 1);
}

FreeTypeFaceWrapper::~FreeTypeFaceWrapper(void)
{
	if(mDoesOwn)
		DoneFace();
	delete mFormatParticularWrapper;
}

static const char* scType1 = "Type 1";
static const char* scTrueType = "TrueType";
static const char* scCFF = "CFF";

void FreeTypeFaceWrapper::SetupFormatSpecificExtender(const string& inPFMFilePath /*pass empty if non existant or irrelevant*/)
{
	if(mFace)
	{
		const char* fontFormat = FT_Get_X11_Font_Format(mFace);

		if(strcmp(fontFormat,scType1) == 0)
			mFormatParticularWrapper = new FreeTypeType1Wrapper(mFace,inPFMFilePath);
		else if(strcmp(fontFormat,scCFF) == 0 || strcmp(fontFormat,scTrueType) == 0)
			mFormatParticularWrapper = new FreeTypeOpenTypeWrapper(mFace);
		else
		{
			mFormatParticularWrapper = NULL;
			TRACE_LOG1("Failure in FreeTypeFaceWrapper::SetupFormatSpecificExtender, could not find format specific implementation for %s",fontFormat);
		}
	}
	else
		mFormatParticularWrapper = NULL;

}

static const char* scEmpty="";
const char* FreeTypeFaceWrapper::GetTypeString()
{
	if(mFace)
	{
		const char* fontFormat = FT_Get_X11_Font_Format(mFace);
		return fontFormat;
	}
	else
	{
		return scEmpty;
	}
}


FT_Face FreeTypeFaceWrapper::operator->()
{
	return mFace;
}

FreeTypeFaceWrapper::operator FT_Face()
{
	return mFace;
}

bool FreeTypeFaceWrapper::IsValid()
{
	return mFace && mFormatParticularWrapper;
}

FT_Error FreeTypeFaceWrapper::DoneFace()
{
	if(mFace)
	{
		FT_Error status = FT_Done_Face(mFace);
		mFace = NULL;
		delete mFormatParticularWrapper;
		mFormatParticularWrapper = NULL;
		return status;
	}
	else
		return 0;
}

double FreeTypeFaceWrapper::GetItalicAngle()
{
	return mFormatParticularWrapper ? mFormatParticularWrapper->GetItalicAngle():0;
}

BoolAndFTShort FreeTypeFaceWrapper::GetCapHeightInternal()
{
	if(mFormatParticularWrapper)
	{
		BoolAndFTShort fontDependentResult = mFormatParticularWrapper->GetCapHeight();
		if(fontDependentResult.first)
			return fontDependentResult;
		else
			return CapHeightFromHHeight();

	}
	else
		return CapHeightFromHHeight();
}

BoolAndFTShort FreeTypeFaceWrapper::GetCapHeight()
{
	BoolAndFTShort result = GetCapHeightInternal();
	if(result.first)
		result.second = GetInPDFMeasurements(result.second);
	return result;
}

BoolAndFTShort FreeTypeFaceWrapper::CapHeightFromHHeight()
{
	// calculate based on Y bearing of the capital H
	return GetYBearingForUnicodeChar(0x48);
}

BoolAndFTShort FreeTypeFaceWrapper::GetxHeightInternal()
{
	if(mFormatParticularWrapper)
	{
		BoolAndFTShort fontDependentResult = mFormatParticularWrapper->GetxHeight();
		if(fontDependentResult.first)
			return fontDependentResult;
		else
			return XHeightFromLowerXHeight();

	}
	else
		return XHeightFromLowerXHeight();
}

BoolAndFTShort FreeTypeFaceWrapper::GetxHeight()
{
	BoolAndFTShort result = GetxHeightInternal();
	if(result.first)
		result.second = GetInPDFMeasurements(result.second);
	return result;
}

BoolAndFTShort FreeTypeFaceWrapper::XHeightFromLowerXHeight()
{
	// calculate based on Y bearing of the lower x
	return GetYBearingForUnicodeChar(0x78);
}

BoolAndFTShort FreeTypeFaceWrapper::GetYBearingForUnicodeChar(unsigned short unicodeCharCode)
{
	if(mFace)
	{
		if(FT_Load_Char(mFace,unicodeCharCode, FT_LOAD_NO_SCALE) != 0)
		{
			TRACE_LOG1("FreeTypeFaceWrapper::XHeightFromLowerXHeight, unable to load glyph for char code = 0x%x",unicodeCharCode);
			return BoolAndFTShort(false,0);
		}
		return BoolAndFTShort(true,(FT_Short)mFace->glyph->metrics.horiBearingY);
	}
	else
		return BoolAndFTShort(false,0);

}

FT_UShort FreeTypeFaceWrapper::GetStemV()
{
	return mFormatParticularWrapper ? GetInPDFMeasurements(mFormatParticularWrapper->GetStemV()):0;
}

EFontStretch FreeTypeFaceWrapper::GetFontStretch()
{
	if(mFormatParticularWrapper)
	{
		EFontStretch result = mFormatParticularWrapper->GetFontStretch();
		if(eFontStretchUknown == result)
			return StretchFromName();
		else
			return result;
	}
	return StretchFromName();
}

EFontStretch FreeTypeFaceWrapper::StretchFromName()
{
	if(mFace)
	{
		if(mFace->style_name)
		{
			if(strstr(mFace->style_name,"Semi Condensed") != NULL)
				return eFontStretchSemiCondensed;

			if(strstr(mFace->style_name,"Ultra Condensed") != NULL || strstr(mFace->style_name,"Extra Compressed") != NULL || strstr(mFace->style_name,"Ultra Compressed") != NULL)
				return eFontStretchUltraCondensed;

			if(strstr(mFace->style_name,"Extra Condensed") != NULL || strstr(mFace->style_name,"Compressed") != NULL)
				return eFontStretchExtraCondensed;

			if(strstr(mFace->style_name,"Condensed") != NULL)
				return eFontStretchCondensed;

			if(strstr(mFace->style_name,"Semi Expanded") != NULL)
				return eFontStretchSemiExpanded;

			if(strstr(mFace->style_name,"Extra Expanded") != NULL)
				return eFontStretchExtraExpanded;

			if(strstr(mFace->style_name,"Ultra Expanded") != NULL)
				return eFontStretchUltraExpanded;

			if(strstr(mFace->style_name,"Expanded") != NULL)
				return eFontStretchExpanded;

			return eFontStretchNormal;
		}
		else
			return eFontStretchNormal;
	}
	else
		return eFontStretchUknown;
}

FT_UShort FreeTypeFaceWrapper::GetFontWeight()
{
	if(mFormatParticularWrapper)
	{
		FT_UShort result = mFormatParticularWrapper->GetFontWeight();
		if(1000 == result) // 1000 marks unknown
			return WeightFromName();
		else
			return result;
	}
	return WeightFromName();
}

FT_UShort FreeTypeFaceWrapper::WeightFromName()
{
	if(mFace)
	{
		if(mFace->style_name)
		{
			if(strstr(mFace->style_name,"Thin") != NULL)
				return 100;

			if(strstr(mFace->style_name,"Black") != NULL || strstr(mFace->style_name,"Heavy") != NULL)
				return 900;

			if(strstr(mFace->style_name,"Extra Light") != NULL || strstr(mFace->style_name,"Ultra Light") != NULL)
				return 200;

			if(strstr(mFace->style_name,"Regular") != NULL || strstr(mFace->style_name,"Normal") != NULL
				|| strstr(mFace->style_name,"Demi Light") != NULL || strstr(mFace->style_name,"Semi Light") != NULL)
				return 400;

			if(strstr(mFace->style_name,"Light") != NULL)
				return 300;

			if(strstr(mFace->style_name,"Medium") != NULL)
				return 500;

			if(strstr(mFace->style_name,"Semi Bold") != NULL || strstr(mFace->style_name,"Demi Bold") != NULL)
				return 600;


			if(strstr(mFace->style_name,"Extra Bold") != NULL || strstr(mFace->style_name,"Ultra Bold") != NULL)
				return 800;

			if(strstr(mFace->style_name,"Bold") != NULL)
				return 700;

			return 400;

		}
		else
			return 400;
	}
	else
		return 1000;
}

unsigned int FreeTypeFaceWrapper::GetFontFlags()
{
	unsigned int flags = 0;

	/*
		flags are a combination of:

		1 - Fixed Pitch
		2 - Serif
		3 - Symbolic
		4 - Script
		6 - Nonsymbolic
		7 - Italic
		17 - AllCap
		18 - SmallCap
		19 - ForceBold

		not doing allcap,smallcap
	*/

	if(IsFixedPitch())
		flags |= 1;
	if(IsSerif())
		flags |= 2;
	if(IsSymbolic())
		flags |= 4;
	else
		flags |= 32;
	if(IsScript())
		flags |= 8;
	if(IsItalic())
		flags |= 64;
	if(IsForceBold())
		flags |= (1<<18);

	return flags;
}

bool FreeTypeFaceWrapper::IsFixedPitch()
{
	return mFace ? FT_IS_FIXED_WIDTH(mFace)!=0 : false;
}

bool FreeTypeFaceWrapper::IsSerif()
{
	return mFormatParticularWrapper ? mFormatParticularWrapper->HasSerifs() : false;
}

bool FreeTypeFaceWrapper::IsSymbolic()
{
	// right now, i have just one method, and it is to query the chars.
	// when i have AFM parser, least i have some info for type 1s

	return IsDefiningCharsNotInAdobeStandardLatin();
}

bool FreeTypeFaceWrapper::IsDefiningCharsNotInAdobeStandardLatin()
{
	if(mFace)
	{
		// loop charachters in font, till you find a non Adobe Standard Latin. hmm. seems like this method marks all as symbol...
		// need to think about this...
		bool hasOnlyAdobeStandard = true;
		FT_ULong characterCode;
		FT_UInt glyphIndex;

		characterCode = FT_Get_First_Char(mFace,&glyphIndex);
		hasOnlyAdobeStandard = IsCharachterCodeAdobeStandard(characterCode);
		while(hasOnlyAdobeStandard && glyphIndex != 0)
		{
			characterCode = FT_Get_Next_Char(mFace, characterCode, &glyphIndex);
			hasOnlyAdobeStandard = IsCharachterCodeAdobeStandard(characterCode);
		}
		return !hasOnlyAdobeStandard;
	}
	else
		return false;
}

bool FreeTypeFaceWrapper::IsCharachterCodeAdobeStandard(FT_ULong inCharacterCode)
{
	// Comparing character code to unicode value of codes in Adobe Standard Latin
	if(inCharacterCode < 0x20) // ignore control charachters
		return true;

	if(betweenIncluding<FT_ULong>(inCharacterCode,0x20,0x7E))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0xA1,0xAC))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0xAE,0xB2))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0xB4,0xBD))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0xBF,0xFF))
		return true;
	if(0x131 == inCharacterCode)
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x141,0x142))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x152,0x153))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x160,0x161))
		return true;
	if(0x178 == inCharacterCode)
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x17D,0x17E))
		return true;
	if(0x192 == inCharacterCode)
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2C6,0x1C7))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2DA,0x1DB))
		return true;
	if(0x2DD == inCharacterCode)
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2D8,0x1D9))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2013,0x2014))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2018,0x201A))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x201C,0x201E))
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2022,0x2021))
		return true;
	if(0x2026 == inCharacterCode)
		return true;
	if(0x2030 == inCharacterCode)
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0x2039,0x203A))
		return true;
	if(0x2044 == inCharacterCode)
		return true;
	if(0x20AC == inCharacterCode)
		return true;
	if(0x2122 == inCharacterCode)
		return true;
	if(betweenIncluding<FT_ULong>(inCharacterCode,0xFB01,0xFB02))
		return true;
	return false;
}

bool FreeTypeFaceWrapper::IsScript()
{
	return mFormatParticularWrapper ? mFormatParticularWrapper->IsScript() : false;
}

bool FreeTypeFaceWrapper::IsItalic()
{
	return GetItalicAngle() != 0;
}

bool FreeTypeFaceWrapper::IsForceBold()
{
	return mFormatParticularWrapper ? mFormatParticularWrapper->IsForceBold() : false;
}

EStatusCode FreeTypeFaceWrapper::GetGlyphsForUnicodeText(const ULongList& inUnicodeCharacters,UIntList& outGlyphs)
{
	if(mFace)
	{
		FT_UInt glyphIndex;
		EStatusCode status = PDFHummus::eSuccess;

		outGlyphs.clear();

		ULongList::const_iterator it = inUnicodeCharacters.begin();
		for(; it != inUnicodeCharacters.end() && PDFHummus::eSuccess == status; ++it)
		{
			glyphIndex = FT_Get_Char_Index(mFace,*it);
			outGlyphs.push_back(glyphIndex);
			if(0 == glyphIndex)
			{
				TRACE_LOG1("FreeTypeFaceWrapper::GetGlyphsForUnicodeText, failed to find glyph for charachter 0x%04x",*it);
				status = PDFHummus::eFailure;
			}
		}

		return status;
	}
	else
		return PDFHummus::eFailure;
}

EStatusCode FreeTypeFaceWrapper::GetGlyphsForUnicodeText(const ULongListList& inUnicodeCharacters,UIntListList& outGlyphs)
{
	UIntList glyphs;
	EStatusCode status = PDFHummus::eSuccess;
	ULongListList::const_iterator it = inUnicodeCharacters.begin();

	for(; it != inUnicodeCharacters.end(); ++it)
	{
		if(PDFHummus::eFailure == GetGlyphsForUnicodeText(*it,glyphs))
			status = PDFHummus::eFailure;
		outGlyphs.push_back(glyphs);
	}

	return status;
}

IWrittenFont* FreeTypeFaceWrapper::CreateWrittenFontObject(ObjectsContext* inObjectsContext)
{
	if(mFace)
	{
		IWrittenFont* result;
		const char* fontFormat = FT_Get_X11_Font_Format(mFace);

		if(strcmp(fontFormat,scType1) == 0 || strcmp(fontFormat,scCFF) == 0)
		{
			FT_Bool isCID = false;

			// CFF written fonts needs to know if the font is originally CID in order to disallow ANSI form in this case
			if(FT_Get_CID_Is_Internally_CID_Keyed(mFace,&isCID) != 0)
				isCID = false;

			result = new WrittenFontCFF(inObjectsContext,isCID != 0);
		}
		else if(strcmp(fontFormat,scTrueType) == 0)
		{
			result = new WrittenFontTrueType(inObjectsContext);
		}
		else
		{
			result = NULL;
			TRACE_LOG1("Failure in FreeTypeFaceWrapper::CreateWrittenFontObject, could not find font writer implementation for %s",
				fontFormat);
		}
		return result;
	}
	else
		return NULL;
}

const string& FreeTypeFaceWrapper::GetFontFilePath()
{
	return mFontFilePath;
}

FT_Short FreeTypeFaceWrapper::GetInPDFMeasurements(FT_Short inFontMeasurement)
{
	if(mFace)
	{
		if(1000 == mFace->units_per_EM)
			return inFontMeasurement;
		else
			return FT_Short((double)inFontMeasurement * 1000.0 / mFace->units_per_EM);
	}
	else
		return 0;
}

FT_UShort FreeTypeFaceWrapper::GetInPDFMeasurements(FT_UShort inFontMeasurement)
{
	if(mFace)
	{
		if(1000 == mFace->units_per_EM)
			return inFontMeasurement;
		else
			return FT_UShort((double)inFontMeasurement * 1000.0 / mFace->units_per_EM);
	}
	else
		return 0;
}

FT_Pos FreeTypeFaceWrapper::GetInPDFMeasurements(FT_Pos inFontMeasurement)
{
	if(mFace)
	{
		if(1000 == mFace->units_per_EM)
			return inFontMeasurement;
		else
			return FT_Pos((double)inFontMeasurement * 1000.0 / mFace->units_per_EM);
	}
	else
		return 0;
}
