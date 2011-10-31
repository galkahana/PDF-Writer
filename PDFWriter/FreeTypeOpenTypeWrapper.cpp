/*
   Source File : FreeTypeOpenTypeWrapper.cpp


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
#include "FreeTypeOpenTypeWrapper.h"
#include "Trace.h"

#include <math.h>

FreeTypeOpenTypeWrapper::FreeTypeOpenTypeWrapper(FT_Face inFace)
{
	void* tableInfo = FT_Get_Sfnt_Table(inFace,ft_sfnt_post);
	if(tableInfo)
	{
		mPostScriptTable = (TT_Postscript*)tableInfo;
	}
	else
	{
		TRACE_LOG("FreeTypeOpenTypeWrapper::FreeTypeOpenTypeWrapper, Warning, Failed to retrieve postscript table");
		mPostScriptTable = NULL;
	}

	tableInfo = FT_Get_Sfnt_Table(inFace,ft_sfnt_os2);
	if(tableInfo)
	{
		mOS2Table = (TT_OS2*)tableInfo;
	}
	else
	{
		TRACE_LOG("FreeTypeOpenTypeWrapper::FreeTypeOpenTypeWrapper, Warning, Failed to retrieve OS2 table");
		mOS2Table = NULL;
	}

	if(!mOS2Table) // don't need the pclt table if i have OS2 (OS2 FTW!!!1)
	{
		tableInfo = FT_Get_Sfnt_Table(inFace,ft_sfnt_pclt);
		if(tableInfo)
		{
			mPCLTTable = (TT_PCLT*)tableInfo;
		}
		else
		{
			TRACE_LOG("FreeTypeOpenTypeWrapper::FreeTypeOpenTypeWrapper, Warning, Failed to retrieve PCLT table");
			mPCLTTable = NULL;
		}
	}
	else
		mPCLTTable = NULL;

	mFace = inFace;
}

FreeTypeOpenTypeWrapper::~FreeTypeOpenTypeWrapper(void)
{
}

double FreeTypeOpenTypeWrapper::GetItalicAngle()
{
	return mPostScriptTable ? SixTeenFloatToDouble(mPostScriptTable->italicAngle) : 0;
}

double FreeTypeOpenTypeWrapper::SixTeenFloatToDouble(FT_Fixed inFixed16_16)
{
	return inFixed16_16 /(double(1<<16));
}

BoolAndFTShort FreeTypeOpenTypeWrapper::GetCapHeight()
{
	if(mOS2Table)
	{
		return BoolAndFTShort(true,mOS2Table->sCapHeight);
	}
	else if (mPCLTTable)
	{
		// converting ushort to short...yikes. hope it ain't a problem. but than it's not supposed to be more than a 1k or something, nah?
		return BoolAndFTShort(true,mPCLTTable->CapHeight);
	}
	else
		return BoolAndFTShort(false,0);
}

BoolAndFTShort FreeTypeOpenTypeWrapper::GetxHeight()
{
	if(mOS2Table)
	{
		return BoolAndFTShort(true,mOS2Table->sxHeight);
	}
	else if (mPCLTTable)
	{
		// converting ushort to short...yikes. hope it ain't a problem. but than it's not supposed to be more than a 1k or something, nah?
		return BoolAndFTShort(true,mPCLTTable->xHeight);
	}
	else
		return BoolAndFTShort(false,0);
}

FT_UShort FreeTypeOpenTypeWrapper::GetStemV()
{
	// For TTF that's a good calculation. For CFFs i should be able to ge this from the private dict information, like the case for type 1 (just use StdVW). for this i'll have to parse the CFF...differ for now
	// instead use the method for true type which would be to try and measure lower case l, and if unsuccesful use some heuristic

	BoolAndFTUShort lowerLWidthResult = StemVFromLowerLWidth();

	if(lowerLWidthResult.first)
	{
		return lowerLWidthResult.second;
	}
	else
	{
		FT_UShort weight;
		if(mOS2Table)
			weight = mOS2Table->usWeightClass;
		else if(mPCLTTable)
			weight = max(mPCLTTable->StrokeWeight * 80 + 500,0) ; // what you see here is an attempt to use linear function to get from strokeweight to weight class
		else
			weight = 500;
		return (FT_UShort)(50 + pow(weight/65.0,2)); // this is some heuristic i found in a web site...lord knows if it's true
	}
}

BoolAndFTUShort FreeTypeOpenTypeWrapper::StemVFromLowerLWidth()
{
	if(FT_Load_Char(mFace,0x6C, FT_LOAD_NO_SCALE) != 0)
	{
		TRACE_LOG("FreeTypeOpenTypeWrapper::StemVFromLowerLWidth, unable to load glyph for lower l");
		return BoolAndFTUShort(false,0);
	}
	return BoolAndFTUShort(true,(FT_UShort)mFace->glyph->metrics.horiAdvance);

	// hmm. i'm getting large numbers with this...maybe this i should prefer the default?
}

EFontStretch FreeTypeOpenTypeWrapper::GetFontStretch()
{
	return
		mOS2Table ?
			(EFontStretch)mOS2Table->usWidthClass :
			mPCLTTable ?
				GetFontStretchForPCLTValue(mPCLTTable->WidthType) :
				eFontStretchUknown;
}


EFontStretch FreeTypeOpenTypeWrapper::GetFontStretchForPCLTValue(FT_Char inWidthValue)
{
	return
		(5 == inWidthValue) ?
			eFontStretchUltraExpanded :
			(-5 == inWidthValue) ?
			eFontStretchUltraCondensed :
			EFontStretch(eFontStretchNormal + inWidthValue);

	// mapping edges to extremes, the rest is linear
}

FT_UShort FreeTypeOpenTypeWrapper::GetFontWeight()
{
	return
		mOS2Table ?
			mOS2Table->usWeightClass :
			mPCLTTable ?
				GetFontWeightFromPCLTValue(mPCLTTable->StrokeWeight) :
				1000;
}

FT_UShort FreeTypeOpenTypeWrapper::GetFontWeightFromPCLTValue(FT_Char inWeightValue)
{
	FT_UShort result;

	switch(inWeightValue)
	{
		case -7:
		case -6:
		case -5:
			result = 100;
			break;
		case -4:
			result = 200;
			break;
		case -3:
			result = 300;
			break;
		case -2:
		case -1:
		case 0:
			result = 400;
			break;
		case 1:
		case 2:
			result = 600;
			break;
		case 3:
			result = 700;
			break;
		case 4:
			result = 800;
			break;
		case 5:
		case 6:
		case 7:
			result = 900;
			break;
		default:
			result = 1000;
	}

	return result;
}

bool FreeTypeOpenTypeWrapper::HasSerifs()
{
	// assume that it does, unless PCLT table says otherwise
	return
		mPCLTTable ?
		((mPCLTTable->SerifStyle>>6) & 3) != 1 :
		true;
}

bool FreeTypeOpenTypeWrapper::IsScript()
{
	return false;
}

bool FreeTypeOpenTypeWrapper::IsForceBold()
{
	return false;
}

