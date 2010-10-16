#include "FreeTypeType1Wrapper.h"
#include "Trace.h"

FreeTypeType1Wrapper::FreeTypeType1Wrapper(FT_Face inFace,const wstring& inPFMFilePath)
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

	mPFMFileInfoRelevant = 
		(inPFMFilePath.size() != 0 && mPFMReader.Read(inPFMFilePath) != eFailure);
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
	if(mPFMFileInfoRelevant)
		return BoolAndFTShort(true,mPFMReader.ExtendedFontMetrics.CapHeight);
	else
		return BoolAndFTShort(false,0);
}

BoolAndFTShort FreeTypeType1Wrapper::GetxHeight()
{
	if(mPFMFileInfoRelevant)
		return BoolAndFTShort(true,mPFMReader.ExtendedFontMetrics.XHeight);
	else
		return BoolAndFTShort(false,0);
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
	if(mPFMFileInfoRelevant)
		return (mPFMReader.Header.PitchAndFamily & 16) != 0;
	else
		return false;

}

bool FreeTypeType1Wrapper::IsScript()
{
	if(mPFMFileInfoRelevant)
		return (mPFMReader.Header.PitchAndFamily & 64) != 0;
	else
		return false;	
}

bool FreeTypeType1Wrapper::IsForceBold()
{
	return mPSPrivateAvailable ? (1 == mPrivateInfo.force_bold) : false;
}
