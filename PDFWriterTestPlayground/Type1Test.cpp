/*
   Source File : Type1Test.cpp


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
#include "Type1Test.h"
#include "TestsRunner.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "CharStringType1Tracer.h"
#include "IByteWriterWithPosition.h"
#include "Type1Input.h"
#include "PSBool.h"

#include <iostream>

using namespace PDFHummus;

Type1Test::Type1Test(void)
{
}

Type1Test::~Type1Test(void)
{
}

EStatusCode Type1Test::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;
	InputFile type1File;
	Type1Input type1Input;

	do
	{
		status = type1File.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFB"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed to open Helvetica input file\n";
			break;
		}

		status = type1Input.ReadType1File(type1File.GetInputStream());
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed to read type 1 file\n";
			break;
		}

		// dump Font dictionary values
		ShowFontDictionary(type1Input.mFontDictionary);
		ShowFontInfoDictionary(type1Input.mFontInfoDictionary);
		ShowPrivateInfoDictionary(type1Input.mPrivateDictionary);


		// calculate dependencies for a,b,c,d and trace their code
		status = ShowDependencies("a",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = ShowDependencies("b",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = ShowDependencies("c",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = ShowDependencies("d",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		// show just abcd and notdef

		status = SaveCharstringCode(inTestConfiguration,".notdef",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = SaveCharstringCode(inTestConfiguration,"a",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = SaveCharstringCode(inTestConfiguration,"b",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = SaveCharstringCode(inTestConfiguration,"c",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;
		status = SaveCharstringCode(inTestConfiguration,"d",&type1Input);
		if(status != PDFHummus::eSuccess)
			break;

	}while(false);

	type1File.CloseFile();
	return  status;
}

void Type1Test::ShowFontDictionary(const Type1FontDictionary& inFontDictionary)
{
	cout<<	"\nShowing Font Dictionary:\n" <<
			"FontName = "<< inFontDictionary.FontName.c_str()<<"\n"<<
			"PaintType = "<<inFontDictionary.PaintType<<"\n"<<
			"FontType = "<<inFontDictionary.FontType<<"\n"<<
			"FontBBox = ";
	ShowDoubleArray(inFontDictionary.FontBBox,4);
	cout<< "\nFontMatrix = ";
	ShowDoubleArray(inFontDictionary.FontMatrix,6);
	cout<< "\nUniqueID = "<< inFontDictionary.UniqueID<<"\n"<<
			"StrokeWidth = "<< inFontDictionary.StrokeWidth<<"\n";

}

void Type1Test::ShowDoubleArray(const double inDoubleArray[],int inSize)
{
	cout<<"[ ";
	for(int i=0;i<inSize;++i)
		cout<<Double(inDoubleArray[i])<<" ";
	cout<<"]";
}


void Type1Test::ShowFontInfoDictionary(const Type1FontInfoDictionary& inFontInfoDictionary)
{
	cout<< "\nShowing Font Info Dictionary:\n"<<
			"version = "<<inFontInfoDictionary.version.c_str()<<"\n"<<
			"Notice = "<<inFontInfoDictionary.Notice.c_str()<<"\n"<<
			"Copyright = "<<inFontInfoDictionary.Copyright.c_str()<<"\n"<<
			"FullName = "<<inFontInfoDictionary.FullName.c_str()<<"\n"<<
			"FamilyName = "<<inFontInfoDictionary.FamilyName.c_str()<<"\n"<<
			"Weight = "<<inFontInfoDictionary.Weight.c_str()<<"\n"<<
			"ItalicAngle = "<<Double(inFontInfoDictionary.ItalicAngle).ToString().c_str()<<"\n"<<
			"isFixedPitch = "<<PSBool(inFontInfoDictionary.isFixedPitch).ToString().c_str()<<"\n"<<
			"UnderlinePosition = "<<Double(inFontInfoDictionary.UnderlinePosition).ToString().c_str()<<"\n"<<
			"UndelineThickness = "<<Double(inFontInfoDictionary.UnderlineThickness).ToString().c_str()<<"\n";
}

void Type1Test::ShowPrivateInfoDictionary(const Type1PrivateDictionary& inPrivateDictionary)
{
	cout<< "\nShowing Private Dictionary:\n"<<
			"UniqueID = "<<inPrivateDictionary.UniqueID<<"\n"<<
			"BlueValues = ";
	ShowIntVector(inPrivateDictionary.BlueValues);
	cout<< "\nOtherBlues = ";
	ShowIntVector(inPrivateDictionary.OtherBlues);
	cout<< "\nFamilyBlues = ";
	ShowIntVector(inPrivateDictionary.FamilyBlues);
	cout<< "\nFamilyOtherBlues = ";
	ShowIntVector(inPrivateDictionary.FamilyOtherBlues);
	cout<< "\nBlueScale = "<<Double(inPrivateDictionary.BlueScale).ToString().c_str()<<"\n"<<
			"BlueShift = "<<inPrivateDictionary.BlueShift<<"\n"<<
			"BlueFuzz = "<<inPrivateDictionary.BlueFuzz<<"\n"<<
			"StdHW = "<<Double(inPrivateDictionary.StdHW).ToString().c_str()<<"\n"<<
			"StdVW = "<<Double(inPrivateDictionary.StdVW).ToString().c_str()<<"\n"<<
			"StemSnapH = ";
	ShowDoubleVector(inPrivateDictionary.StemSnapH);
	cout<< "\nStemSnapW = ";
	ShowDoubleVector(inPrivateDictionary.StemSnapV);
	cout<< "\nForceBold = "<<PSBool(inPrivateDictionary.ForceBold).ToString().c_str()<<"\n"<<
			"LanguageGrup = "<<inPrivateDictionary.LanguageGroup<<"\n"<<
			"lenIV = "<<inPrivateDictionary.lenIV<<"\n"<<
			"RndStemUp = "<<PSBool(inPrivateDictionary.RndStemUp).ToString().c_str()<<"\n";
}

void Type1Test::ShowIntVector(const vector<int>& inVector)
{
	vector<int>::const_iterator it = inVector.begin();

	cout<<"[ ";
	for(;it != inVector.end();++it)
		cout<<*it<<" ";
	cout<<"]";	
}

void Type1Test::ShowDoubleVector(const vector<double>& inVector)
{
	vector<double>::const_iterator it = inVector.begin();

	cout<<"[ ";
	for(;it != inVector.end();++it)
		cout<<Double(*it).ToString().c_str()<<" ";
	cout<<"]";	
}

EStatusCode Type1Test::ShowDependencies(const string& inCharStringName,Type1Input* inType1Input)
{
	CharString1Dependencies dependencies;
	EStatusCode status = inType1Input->CalculateDependenciesForCharIndex(inCharStringName,dependencies);

	if(status != PDFHummus::eSuccess)
	{
		cout<<"count not calculate dependencies for glyph "<<inCharStringName.c_str()<<"\n";
		return PDFHummus::eFailure;
	}

	if(dependencies.mCharCodes.size() != 0 || dependencies.mOtherSubrs.size() != 0 || dependencies.mSubrs.size() != 0)
	{
		cout<<"Glyph "<<inCharStringName.c_str()<<" has dependencies:\n";
		ByteSet::iterator itBytes = dependencies.mCharCodes.begin();
		for(; itBytes != dependencies.mCharCodes.end(); ++itBytes)
			cout<<"Seac Charcode "<<*itBytes<<"\n";
		UShortSet::iterator itShort = dependencies.mOtherSubrs.begin();
		for(; itShort != dependencies.mOtherSubrs.end(); ++itShort)
			cout<<"Other Subr "<<*itShort<<"\n";
		itShort = dependencies.mSubrs.begin();
		for(; itShort != dependencies.mSubrs.end(); ++itShort)
			cout<<"Subr "<<*itShort<<"\n";
	}
	else
	{
		cout<<"No dependencies for "<<inCharStringName.c_str()<<"\n";
	}
	return PDFHummus::eSuccess;
}

EStatusCode Type1Test::SaveCharstringCode(const TestConfiguration& inTestConfiguration,const string& inCharStringName,Type1Input* inType1Input)
{
	OutputFile glyphFile;

    
	EStatusCode status = glyphFile.OpenFile(
                                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("glyphType1_") + inCharStringName + "_.txt"));

	do
	{
		if(status != PDFHummus::eSuccess)
			break;
		
		CharStringType1Tracer tracer;

		status = tracer.TraceGlyphProgram(inCharStringName,inType1Input,glyphFile.GetOutputStream());

	}while(false);

	glyphFile.CloseFile();

	return status;

}

ADD_CATEGORIZED_TEST(Type1Test,"Type1")
