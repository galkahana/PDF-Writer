#include "FreeTypeType1Wrapper.h"
#include "Trace.h"

FreeTypeType1Wrapper::FreeTypeType1Wrapper(FT_Face inFace)
{
	if(FT_Get_PS_Font_Info(inFace,&mPSFontInfo) != 0)
	{
		TRACE_LOG("Unexpected failure in FreeTypeType1Wrapper::FreeTypeType1Wrapper. could not retrieve PS font info");
		mPSavailable = false; // this is obviousy an exception
	}
	else
		mPSavailable = true; 


	if(FT_Get_PS_Font_Private(inFace,&mPrivateInfo) != 0)
	{
		TRACE_LOG("Unexpected failure in FreeTypeType1Wrapper::FreeTypeType1Wrapper. could not retrieve PS private font info");
		mPSPrivateAvailable = false; // this is obviousy an exception
	}
	else
		mPSPrivateAvailable = true; 
}

FreeTypeType1Wrapper::~FreeTypeType1Wrapper(void)
{
}

double FreeTypeType1Wrapper::GetItalicAngle()
{
	return mPSavailable ? (double)mPSFontInfo.italic_angle : 0;
}

BoolAndFTShort FreeTypeType1Wrapper::GetCapHeight()
{
	// hmm, free type doesn't have interface for PFM files
	// so i'll default to no info. now.

	return BoolAndFTShort(false,0);
}

BoolAndFTShort FreeTypeType1Wrapper::GetxHeight()
{
	// probably in PFM, but no interface from Free type. default to no info
	return BoolAndFTShort(false,0);

	// yeah...looks like parsing PFM/AFM would be propper
}

FT_UShort FreeTypeType1Wrapper::GetStemV()
{
	// StemV of PDF matches the StdWV of type 1...so piece of cake here
	return mPSPrivateAvailable ? mPrivateInfo.standard_width[0]:0;
}

EFontStretch FreeTypeType1Wrapper::GetFontStretch()
{
	return eFontStretchUknown;
}

FT_UShort FreeTypeType1Wrapper::GetFontWeight()
{
	// ahhh. using the weight factor is as good as analyzing the name...whatever. so just return the default value
	return 1000;
}

bool FreeTypeType1Wrapper::HasSerifs()
{
	// TODO: need to read from PFM dfPitchAndFamily and determine with that.
	// for now default to false [and then]
	return false;
}

bool FreeTypeType1Wrapper::IsForceBold()
{
	return mPSPrivateAvailable ? (1 == mPrivateInfo.force_bold) : false;
}
