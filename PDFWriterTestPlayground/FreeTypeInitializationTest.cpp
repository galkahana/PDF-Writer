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
using namespace PDFHummus;

FreeTypeInitializationTest::FreeTypeInitializationTest(void)
{
}

FreeTypeInitializationTest::~FreeTypeInitializationTest(void)
{
}

EStatusCode FreeTypeInitializationTest::Run(const TestConfiguration& inTestConfiguration)
{
	Singleton<Trace>::GetInstance()->SetLogSettings(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"FreeTypeTest.txt"),true,true);

	EStatusCode status = PDFHummus::eSuccess;
	FreeTypeWrapper ftWrapper;


	do
	{
		status = ShowFaceProperties(ftWrapper,
                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
		if(status != PDFHummus::eSuccess)
			break;

		status = ShowFaceProperties(ftWrapper,
                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFB"),
                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFM"));
		if(status != PDFHummus::eSuccess)
			break;

		status = ShowFaceProperties(ftWrapper,
                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/BrushScriptStd.otf"));
		if(status != PDFHummus::eSuccess)
			break;

	}while(false);


	Singleton<Trace>::Reset();

	return status;
}

EStatusCode FreeTypeInitializationTest::ShowFaceProperties(FreeTypeWrapper& inFreeType,const string& inFontFilePath,const string& inSecondaryFontFilePath)
{
	FT_Face face;
	EStatusCode status = PDFHummus::eSuccess;

	do
	{
		cout<<"Start Font\n";
		if(inSecondaryFontFilePath.length() > 0)
			face = inFreeType.NewFace(inFontFilePath,inSecondaryFontFilePath,0);
		else
			face = inFreeType.NewFace(inFontFilePath,0);
		if(!face)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to load font from "<<inFontFilePath.c_str()<<"\n";
			break;
		}
		status = ShowGlobalFontProperties(inFreeType,face);
	}
	while(false);

	inFreeType.DoneFace(face);
	cout<<"End Font\n\n";
	return status;
}

EStatusCode FreeTypeInitializationTest::ShowGlobalFontProperties(FreeTypeWrapper& inFreeType,FT_Face inFace)
{
	EStatusCode status = PDFHummus::eSuccess;

	FreeTypeFaceWrapper face(inFace,"",0,false);

	cout<<"Font Family = "<<(face->family_name ? face->family_name : "somefont")<<"\n";
	cout<<"Font Style = "<<(face->style_name ? face->style_name : "somestyle")<<"\n";
	cout<<"Font Format = "<<FT_Get_X11_Font_Format(face)<<"\n";
	cout<<"Font CID Keyed (does not includes sfnts) = "<<(face->face_flags & FT_FACE_FLAG_CID_KEYED ? "yes":"no")<<"\n";
	cout<<"Font SFNT modeled = "<<(FT_IS_SFNT(face) ? "yes":"no")<<"\n";
	if(FT_IS_SFNT(face))
	{
		FT_ULong  length = 0;
		if(FT_Load_Sfnt_Table(face,TTAG_CFF,0,NULL,&length) == 0)
			cout<<"CCF table found, length is "<<length<<"\n";
	}
	
	FT_Bool isCID;
	
	if(FT_Get_CID_Is_Internally_CID_Keyed(face,&isCID) != 0)
	{
		cout<<"No CID information to read\n";
		isCID = false;
	}
	else
		cout<<"Font Internally CID (checks also sfnts) = "<<(isCID ? "yes":"no")<<"\n";
	
	if(isCID)
	{
		const char* registry;
		const char* ordering;
		FT_Int supplement;
		if(FT_Get_CID_Registry_Ordering_Supplement(face,&registry,&ordering,&supplement) != 0)
		{
			cout<<"Failed to read registry, ordering and supplement informaiton\n";
			status = PDFHummus::eFailure;
		}
		cout<<"CID Registry = "<<registry<<"\n";
		cout<<"CID Ordering = "<<ordering<<"\n";
		cout<<"CID Supplement = "<<supplement<<"\n";
	}

	cout<<"Font BBox = ["<<face->bbox.xMin<<" "<<face->bbox.yMin<<" "<<face->bbox.xMax<<" "<<face->bbox.yMax<<"]\n";
	cout<<"Ascent "<<face->ascender<<"\n";
	cout<<"Descent "<<face->descender<<"\n";
	cout<<"Italic Angle = "<<face.GetItalicAngle()<<"\n";
	BoolAndFTShort capHeightResult = face.GetCapHeight();
	if(capHeightResult.first)
		cout<<"Cap Height = "<<capHeightResult.second<<"\n";
	else
		cout<<"No Cap Height value\n";
	BoolAndFTShort xHeightResult = face.GetxHeight();
	if(capHeightResult.first)
		cout<<"x Height = "<<xHeightResult.second<<"\n";
	else
		cout<<"No x Height value\n";
	cout<<"StemV = "<<face.GetStemV()<<"\n";
	EFontStretch fontStretch = face.GetFontStretch();
	if(eFontStretchUknown == fontStretch)
		cout<<"Unkown Stretch";
	else
		cout<<"Stretch = "<<fontStretch<<"\n";

	FT_UShort fontWeight = face.GetFontWeight();
	if(1000 == fontWeight)
		cout<<"Unknown Weight";
	else
		cout<<"Weight = "<<fontWeight<<"\n";
	cout<<"FontFlags = "<<face.GetFontFlags()<<"\n";
	if(FT_HAS_GLYPH_NAMES(face))
	{
		FT_UInt glyphIndex = FT_Get_Char_Index(face,0x31);

		cout<<"Font has glyph names\n";
		if(glyphIndex != 0)
		{
			char buffer[31];
			if(FT_Get_Glyph_Name(face,glyphIndex,buffer,31) == 0)
				cout<<"Glyph name for the 1 = "<<buffer<<"\n";
		}
	}
	else
	{
		cout<<"Font does not have glyph names\n";
	}
	return status;
}

ADD_CATEGORIZED_TEST(FreeTypeInitializationTest,"FreeType")
