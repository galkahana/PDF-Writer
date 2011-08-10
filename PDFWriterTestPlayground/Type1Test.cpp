/*
   Source File : Type1Test.cpp


   Copyright 2011 Gal Kahana HummusPDFWriter

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
#include "StringTraits.h"
#include "IByteWriterWithPosition.h"
#include "Type1Input.h"
#include "PSBool.h"

#include <iostream>

Type1Test::Type1Test(void)
{
}

Type1Test::~Type1Test(void)
{
}

EPDFStatusCode Type1Test::Run()
{
	EPDFStatusCode status = ePDFSuccess;
	InputFile type1File;
	Type1Input type1Input;

	do
	{
		status = type1File.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFB");
		if(status != ePDFSuccess)
		{
			wcout<<"Failed to open Helvetica input file\n";
			break;
		}

		status = type1Input.ReadType1File(type1File.GetInputStream());
		if(status != ePDFSuccess)
		{
			wcout<<"Failed to read type 1 file\n";
			break;
		}

		// dump Font dictionary values
		ShowFontDictionary(type1Input.mFontDictionary);
		ShowFontInfoDictionary(type1Input.mFontInfoDictionary);
		ShowPrivateInfoDictionary(type1Input.mPrivateDictionary);


		// calculate dependencies for a,b,c,d and trace their code
		status = ShowDependencies("a",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = ShowDependencies("b",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = ShowDependencies("c",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = ShowDependencies("d",&type1Input);
		if(status != ePDFSuccess)
			break;
		// show just abcd and notdef

		status = SaveCharstringCode(".notdef",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = SaveCharstringCode("a",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = SaveCharstringCode("b",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = SaveCharstringCode("c",&type1Input);
		if(status != ePDFSuccess)
			break;
		status = SaveCharstringCode("d",&type1Input);
		if(status != ePDFSuccess)
			break;

	}while(false);

	type1File.CloseFile();
	return  status;
}

void Type1Test::ShowFontDictionary(const Type1FontDictionary& inFontDictionary)
{
	wcout<<	L"\nShowing Font Dictionary:\n" <<
			L"FontName = "<< StringTraits(inFontDictionary.FontName).WidenString().c_str()<<L"\n"<<
			L"PaintType = "<<inFontDictionary.PaintType<<L"\n"<<
			L"FontType = "<<inFontDictionary.FontType<<L"\n"<<
			L"FontBBox = ";
	ShowDoubleArray(inFontDictionary.FontBBox,4);
	wcout<< L"\nFontMatrix = ";
	ShowDoubleArray(inFontDictionary.FontMatrix,6);
	wcout<< L"\nUniqueID = "<< inFontDictionary.UniqueID<<L"\n"<<
			L"StrokeWidth = "<< inFontDictionary.StrokeWidth<<L"\n";

}

void Type1Test::ShowDoubleArray(const double inDoubleArray[],int inSize)
{
	wcout<<L"[ ";
	for(int i=0;i<inSize;++i)
		wcout<<Double(inDoubleArray[i])<<L" ";
	wcout<<L"]";
}


void Type1Test::ShowFontInfoDictionary(const Type1FontInfoDictionary& inFontInfoDictionary)
{
	wcout<< L"\nShowing Font Info Dictionary:\n"<<
			L"version = "<<StringTraits(inFontInfoDictionary.version).WidenString().c_str()<<L"\n"<<
			L"Notice = "<<StringTraits(inFontInfoDictionary.Notice).WidenString().c_str()<<L"\n"<<
			L"Copyright = "<<StringTraits(inFontInfoDictionary.Copyright).WidenString().c_str()<<L"\n"<<
			L"FullName = "<<StringTraits(inFontInfoDictionary.FullName).WidenString().c_str()<<L"\n"<<
			L"FamilyName = "<<StringTraits(inFontInfoDictionary.FamilyName).WidenString().c_str()<<L"\n"<<
			L"Weight = "<<StringTraits(inFontInfoDictionary.Weight).WidenString().c_str()<<L"\n"<<
			L"ItalicAngle = "<<Double(inFontInfoDictionary.ItalicAngle).ToWString().c_str()<<L"\n"<<
			L"isFixedPitch = "<<PSBool(inFontInfoDictionary.isFixedPitch).ToWString().c_str()<<L"\n"<<
			L"UnderlinePosition = "<<Double(inFontInfoDictionary.UnderlinePosition).ToWString().c_str()<<L"\n"<<
			L"UndelineThickness = "<<Double(inFontInfoDictionary.UnderlineThickness).ToWString().c_str()<<L"\n";
}

void Type1Test::ShowPrivateInfoDictionary(const Type1PrivateDictionary& inPrivateDictionary)
{
	wcout<< L"\nShowing Private Dictionary:\n"<<
			L"UniqueID = "<<inPrivateDictionary.UniqueID<<L"\n"<<
			L"BlueValues = ";
	ShowIntVector(inPrivateDictionary.BlueValues);
	wcout<< L"\nOtherBlues = ";
	ShowIntVector(inPrivateDictionary.OtherBlues);
	wcout<< L"\nFamilyBlues = ";
	ShowIntVector(inPrivateDictionary.FamilyBlues);
	wcout<< L"\nFamilyOtherBlues = ";
	ShowIntVector(inPrivateDictionary.FamilyOtherBlues);
	wcout<< L"\nBlueScale = "<<Double(inPrivateDictionary.BlueScale).ToWString().c_str()<<L"\n"<<
			L"BlueShift = "<<inPrivateDictionary.BlueShift<<L"\n"<<
			L"BlueFuzz = "<<inPrivateDictionary.BlueFuzz<<L"\n"<<
			L"StdHW = "<<Double(inPrivateDictionary.StdHW).ToWString().c_str()<<L"\n"<<
			L"StdVW = "<<Double(inPrivateDictionary.StdVW).ToWString().c_str()<<L"\n"<<
			L"StemSnapH = ";
	ShowDoubleVector(inPrivateDictionary.StemSnapH);
	wcout<< L"\nStemSnapW = ";
	ShowDoubleVector(inPrivateDictionary.StemSnapV);
	wcout<< L"\nForceBold = "<<PSBool(inPrivateDictionary.ForceBold).ToWString().c_str()<<L"\n"<<
			L"LanguageGrup = "<<inPrivateDictionary.LanguageGroup<<L"\n"<<
			L"lenIV = "<<inPrivateDictionary.lenIV<<L"\n"<<
			L"RndStemUp = "<<PSBool(inPrivateDictionary.RndStemUp).ToWString().c_str()<<L"\n";
}

void Type1Test::ShowIntVector(const vector<int>& inVector)
{
	vector<int>::const_iterator it = inVector.begin();

	wcout<<L"[ ";
	for(;it != inVector.end();++it)
		wcout<<*it<<L" ";
	wcout<<L"]";	
}

void Type1Test::ShowDoubleVector(const vector<double>& inVector)
{
	vector<double>::const_iterator it = inVector.begin();

	wcout<<L"[ ";
	for(;it != inVector.end();++it)
		wcout<<Double(*it).ToWString().c_str()<<L" ";
	wcout<<L"]";	
}

EPDFStatusCode Type1Test::ShowDependencies(const string& inCharStringName,Type1Input* inType1Input)
{
	CharString1Dependencies dependencies;
	EPDFStatusCode status = inType1Input->CalculateDependenciesForCharIndex(inCharStringName,dependencies);

	if(status != ePDFSuccess)
	{
		wcout<<"count not calculate dependencies for glyph "<<inCharStringName.c_str()<<"\n";
		return ePDFFailure;
	}

	if(dependencies.mCharCodes.size() != 0 || dependencies.mOtherSubrs.size() != 0 || dependencies.mSubrs.size() != 0)
	{
		wcout<<"Glyph "<<inCharStringName.c_str()<<" has dependencies:\n";
		ByteSet::iterator itBytes = dependencies.mCharCodes.begin();
		for(; itBytes != dependencies.mCharCodes.end(); ++itBytes)
			wcout<<"Seac Charcode "<<*itBytes<<"\n";
		UShortSet::iterator itShort = dependencies.mOtherSubrs.begin();
		for(; itShort != dependencies.mOtherSubrs.end(); ++itShort)
			wcout<<"Other Subr "<<*itShort<<"\n";
		itShort = dependencies.mSubrs.begin();
		for(; itShort != dependencies.mSubrs.end(); ++itShort)
			wcout<<"Subr "<<*itShort<<"\n";
	}
	else
	{
		wcout<<"No dependencies for "<<inCharStringName.c_str()<<"\n";
	}
	return ePDFSuccess;
}

EPDFStatusCode Type1Test::SaveCharstringCode(const string& inCharStringName,Type1Input* inType1Input)
{
	OutputFile glyphFile;

	EPDFStatusCode status = glyphFile.OpenFile(wstring(L"C:\\PDFLibTests\\glyphType1_") + StringTraits(inCharStringName).WidenString() + L"_.txt");

	do
	{
		if(status != ePDFSuccess)
			break;
		
		CharStringType1Tracer tracer;

		status = tracer.TraceGlyphProgram(inCharStringName,inType1Input,glyphFile.GetOutputStream());

	}while(false);

	glyphFile.CloseFile();

	return status;

}

ADD_CATEGORIZED_TEST(Type1Test,"Type1")
