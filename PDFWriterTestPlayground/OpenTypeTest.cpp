#include "OpenTypeTest.h"
#include "OpenTypeFileInput.h"
#include "InputFile.h"
#include "TestsRunner.h"
#include "CFFFileInput.h"

#include "BoxingBase.h"
#include "OutputFile.h"
#include "CharStringType2Tracer.h"
#include "IByteWriterWithPosition.h"

#include <iostream>


OpenTypeTest::OpenTypeTest(void)
{
}

OpenTypeTest::~OpenTypeTest(void)
{
}


EStatusCode OpenTypeTest::SaveCharstringCode(unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput)
{
	OutputFile glyphFile;

	EStatusCode status = glyphFile.OpenFile(wstring(L"C:\\PDFLibTests\\glyph")  + Long(inFontIndex).ToWString() + L"_" + Long(inGlyphIndex).ToWString() + L".txt");

	do
	{
		if(status != eSuccess)
			break;
		
		CharStringType2Tracer tracer;

		status = tracer.TraceGlyphProgram(inFontIndex,inGlyphIndex,inCFFFileInput,glyphFile.GetOutputStream());

	}while(false);

	glyphFile.CloseFile();

	return status;
}

EStatusCode OpenTypeTest::Run()
{
	return TestFont();
}

EStatusCode OpenTypeTest::TestFont()
{
	EStatusCode status;
	InputFile otfFile;

	do
	{
		status = otfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\BrushScriptStd.otf");

		if(status != eSuccess)
		{
			wcout<<"cannot read bursh script font file\n";
			break;
		}


		OpenTypeFileInput openTypeReader;

		status = openTypeReader.ReadOpenTypeFile(otfFile.GetInputStream());
		if(status != eSuccess)
		{
			wcout<<"could not read open type file\n";
			break;
		}

		/*
		unsigned short glyphsCount = openTypeReader.mCFF.GetCharStringsCount(0);
		for(unsigned short i=0; i < glyphsCount && eSuccess == status; ++i)
		{
			CharString2Dependencies dependencies;
			status = openTypeReader.mCFF.CalculateDependenciesForCharIndex(0,i,dependencies);
			if(status != eSuccess)
			{
				wcout<<"count not calculate dependencies for glyph index "<<i<<"\n";
			}
			else
			{
				if(dependencies.mLocalSubrs.size() != 0 || dependencies.mGlobalSubrs.size() != 0)
				{
					wcout<<"Glyph index = "<<i<<", Named "<<openTypeReader.mCFF.GetGlyphName(0,i).c_str()<<" has subroutine dependencies:\n";
					UShortSet::iterator it;
					for(it = dependencies.mGlobalSubrs.begin(); it != dependencies.mGlobalSubrs.end(); ++it)
						wcout<<"Global Subr, Index = "<<*it<<"\n";
					for(it = dependencies.mLocalSubrs.begin(); it != dependencies.mLocalSubrs.end(); ++it)
						wcout<<"Local Subr, Index = "<<*it<<"\n";
				}
				else
				{
					wcout<<"No subrs dependencies for Glyph index = "<<i<<", Named "<<openTypeReader.mCFF.GetGlyphName(0,i).c_str()<<"\n";
				}
			}
		}*/

		// show just abcd and notdef

		status = SaveCharstringCode(0,0,&openTypeReader.mCFF);
		for(unsigned short i=66; i < 70 && eSuccess == status; ++i)
			status = SaveCharstringCode(0,i,&openTypeReader.mCFF);
	}while(false);
	return status;
}

ADD_CETEGORIZED_TEST(OpenTypeTest,"OpenType")