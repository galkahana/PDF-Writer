#include "Type1Test.h"
#include "TestsRunner.h"
#include "InputFile.h"
#include "Type1Input.h"
#include "OutputFile.h"
#include "CharStringType1Tracer.h"
#include "StringTraits.h"
#include "IByteWriterWithPosition.h"

#include <iostream>

Type1Test::Type1Test(void)
{
}

Type1Test::~Type1Test(void)
{
}

EStatusCode Type1Test::Run()
{
	EStatusCode status = eSuccess;
	InputFile type1File;
	Type1Input type1Input;

	do
	{
		status = type1File.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFB");
		if(status != eSuccess)
		{
			wcout<<"Failed to open Helvetica input file\n";
			break;
		}

		status = type1Input.ReadType1File(type1File.GetInputStream());
		if(status != eSuccess)
		{
			wcout<<"Failed to read type 1 file\n";
			break;
		}

		// calculate dependencies for a,b,c,d and trace their code
		status = ShowDependencies("a",&type1Input);
		if(status != eSuccess)
			break;
		status = ShowDependencies("b",&type1Input);
		if(status != eSuccess)
			break;
		status = ShowDependencies("c",&type1Input);
		if(status != eSuccess)
			break;
		status = ShowDependencies("d",&type1Input);
		if(status != eSuccess)
			break;
		// show just abcd and notdef

		status = SaveCharstringCode("a",&type1Input);
		if(status != eSuccess)
			break;
		status = SaveCharstringCode("b",&type1Input);
		if(status != eSuccess)
			break;
		status = SaveCharstringCode("c",&type1Input);
		if(status != eSuccess)
			break;
		status = SaveCharstringCode("d",&type1Input);
		if(status != eSuccess)
			break;
		
	}while(false);


	type1File.CloseFile();
	return  status;
}


EStatusCode Type1Test::ShowDependencies(const string& inCharStringName,Type1Input* inType1Input)
{
	CharString1Dependencies dependencies;
	EStatusCode status = inType1Input->CalculateDependenciesForCharIndex(inCharStringName,dependencies);

	if(status != eSuccess)
	{
		wcout<<"count not calculate dependencies for glyph "<<inCharStringName.c_str()<<"\n";
		return eFailure;
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
	return eSuccess;
}

EStatusCode Type1Test::SaveCharstringCode(const string& inCharStringName,Type1Input* inType1Input)
{
	OutputFile glyphFile;

	EStatusCode status = glyphFile.OpenFile(wstring(L"C:\\PDFLibTests\\glyphType1_") + StringTraits(inCharStringName).WidenString() + L"_.txt");

	do
	{
		if(status != eSuccess)
			break;
		
		CharStringType1Tracer tracer;

		status = tracer.TraceGlyphProgram(inCharStringName,inType1Input,glyphFile.GetOutputStream());

	}while(false);

	glyphFile.CloseFile();

	return status;

}

ADD_CETEGORIZED_TEST(Type1Test,"Type1")