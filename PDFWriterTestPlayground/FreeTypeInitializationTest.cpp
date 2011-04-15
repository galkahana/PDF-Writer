/*
   Source File : FreeTypeInitializationTest.cpp


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
#include "FreeTypeInitializationTest.h"

#include "FreeTypeFaceWrapper.h"
#include "EFontStretch.h"

#include "Trace.h"
#include "Singleton.h"

#include FT_XFREE86_H 
#include FT_CID_H 
#include FT_TYPE1_TABLES_H 
#include FT_TRUETYPE_TAGS_H
#include FT_TRUETYPE_TABLES_H 

#include <iostream>

using namespace std;

FreeTypeInitializationTest::FreeTypeInitializationTest(void)
{
}

FreeTypeInitializationTest::~FreeTypeInitializationTest(void)
{
}

EStatusCode FreeTypeInitializationTest::Run()
{
	Singleton<Trace>::GetInstance()->SetLogSettings(L"c:\\PDFLibTests\\FreeTypeTest.txt",true);

	EStatusCode status = eSuccess;
	FreeTypeWrapper ftWrapper;


	do
	{
		status = ShowFaceProperties(ftWrapper,L"C:\\PDFLibTests\\TestMaterials\\fonts\\arial.ttf");
		if(status != eSuccess)
			break;

		status = ShowFaceProperties(ftWrapper,L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFB",L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFM");
		if(status != eSuccess)
			break;

		status = ShowFaceProperties(ftWrapper,L"C:\\PDFLibTests\\TestMaterials\\fonts\\BrushScriptStd.otf");
		if(status != eSuccess)
			break;

	}while(false);


	Singleton<Trace>::Reset();

	return status;
}

EStatusCode FreeTypeInitializationTest::ShowFaceProperties(FreeTypeWrapper& inFreeType,const wchar_t* inFontFilePath,const wchar_t* inSecondaryFontFilePath)
{
	FT_Face face;
	EStatusCode status = eSuccess;

	do
	{
		wcout<<"Start Font\n";
		if(inSecondaryFontFilePath)
			face = inFreeType.NewFace(inFontFilePath,inSecondaryFontFilePath);
		else
			face = inFreeType.NewFace(inFontFilePath);
		if(!face)
		{
			status = eFailure;
			wcout<<"Failed to load font from "<<inFontFilePath<<"\n";
			break;
		}
		status = ShowGlobalFontProperties(inFreeType,face);
	}
	while(false);

	inFreeType.DoneFace(face);
	wcout<<"End Font\n\n";
	return status;
}

EStatusCode FreeTypeInitializationTest::ShowGlobalFontProperties(FreeTypeWrapper& inFreeType,FT_Face inFace)
{
	EStatusCode status = eSuccess;

	FreeTypeFaceWrapper face(inFace,L"",false);

	wcout<<"Font Family = "<<(face->family_name ? face->family_name : "somefont")<<"\n";
	wcout<<"Font Style = "<<(face->style_name ? face->style_name : "somestyle")<<"\n";
	wcout<<"Font Format = "<<FT_Get_X11_Font_Format(face)<<"\n";
	wcout<<"Font CID Keyed (does not includes sfnts) = "<<(face->face_flags & FT_FACE_FLAG_CID_KEYED ? "yes":"no")<<"\n";
	wcout<<"Font SFNT modeled = "<<(FT_IS_SFNT(face) ? "yes":"no")<<"\n";
	if(FT_IS_SFNT(face))
	{
		FT_ULong  length = 0;
		if(FT_Load_Sfnt_Table(face,TTAG_CFF,0,NULL,&length) == 0)
			wcout<<"CCF table found, length is "<<length<<"\n";
	}
	
	FT_Bool isCID;
	
	if(FT_Get_CID_Is_Internally_CID_Keyed(face,&isCID) != 0)
	{
		wcout<<"No CID information to read\n";
		isCID = false;
	}
	else
		wcout<<"Font Internally CID (checks also sfnts) = "<<(isCID ? "yes":"no")<<"\n";
	
	if(isCID)
	{
		const char* registry;
		const char* ordering;
		FT_Int supplement;
		if(FT_Get_CID_Registry_Ordering_Supplement(face,&registry,&ordering,&supplement) != 0)
		{
			wcout<<"Failed to read registry, ordering and supplement informaiton\n";
			status = eFailure;
		}
		wcout<<"CID Registry = "<<registry<<"\n";
		wcout<<"CID Ordering = "<<ordering<<"\n";
		wcout<<"CID Supplement = "<<supplement<<"\n";
	}

	wcout<<"Font BBox = ["<<face->bbox.xMin<<" "<<face->bbox.yMin<<" "<<face->bbox.xMax<<" "<<face->bbox.yMax<<"]\n";
	wcout<<"Ascent "<<face->ascender<<"\n";
	wcout<<"Descent "<<face->descender<<"\n";
	wcout<<"Italic Angle = "<<face.GetItalicAngle()<<"\n";
	BoolAndFTShort capHeightResult = face.GetCapHeight();
	if(capHeightResult.first)
		wcout<<"Cap Height = "<<capHeightResult.second<<"\n";
	else
		wcout<<"No Cap Height value\n";
	BoolAndFTShort xHeightResult = face.GetxHeight();
	if(capHeightResult.first)
		wcout<<"x Height = "<<xHeightResult.second<<"\n";
	else
		wcout<<"No x Height value\n";
	wcout<<"StemV = "<<face.GetStemV()<<"\n";
	EFontStretch fontStretch = face.GetFontStretch();
	if(eFontStretchUknown == fontStretch)
		wcout<<"Unkown Stretch";
	else
		wcout<<"Stretch = "<<scFontStretchLabels[fontStretch]<<"\n";

	FT_UShort fontWeight = face.GetFontWeight();
	if(1000 == fontWeight)
		wcout<<"Unknown Weight";
	else
		wcout<<"Weight = "<<fontWeight<<"\n";
	wcout<<"FontFlags = "<<face.GetFontFlags()<<"\n";
	if(FT_HAS_GLYPH_NAMES(face))
	{
		FT_UInt glyphIndex = FT_Get_Char_Index(face,0x31);

		wcout<<"Font has glyph names\n";
		if(glyphIndex != 0)
		{
			char buffer[31];
			if(FT_Get_Glyph_Name(face,glyphIndex,buffer,31) == 0)
				wcout<<"Glyph name for the 1 = "<<buffer<<"\n";
		}
	}
	else
	{
		wcout<<"Font does not have glyph names\n";
	}
	return status;
}

ADD_CATEGORIZED_TEST(FreeTypeInitializationTest,"FreeType")
