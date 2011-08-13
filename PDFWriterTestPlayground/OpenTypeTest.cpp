/*
   Source File : OpenTypeTest.cpp


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
#include "OpenTypeTest.h"
#include "OpenTypeFileInput.h"
#include "InputFile.h"
#include "TestsRunner.h"

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

EPDFStatusCode OpenTypeTest::SaveCharstringCode(unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput)
{
	OutputFile glyphFile;

	EPDFStatusCode status = glyphFile.OpenFile(string("C:\\PDFLibTests\\glyphCFF")  + Long(inFontIndex).ToString() + "_" + inCFFFileInput->GetGlyphName(0,inGlyphIndex) + ".txt");

	do
	{
		if(status != ePDFSuccess)
			break;
		
		CharStringType2Tracer tracer;

		status = tracer.TraceGlyphProgram(inFontIndex,inGlyphIndex,inCFFFileInput,glyphFile.GetOutputStream());

	}while(false);

	glyphFile.CloseFile();

	return status;
}

EPDFStatusCode OpenTypeTest::Run()
{
	return TestFont();
}

EPDFStatusCode OpenTypeTest::TestFont()
{
	EPDFStatusCode status;
	InputFile otfFile;

	do
	{
		status = otfFile.OpenFile("C:\\PDFLibTests\\TestMaterials\\fonts\\BrushScriptStd.otf");

		if(status != ePDFSuccess)
		{
			cout<<"cannot read bursh script font file\n";
			break;
		}


		OpenTypeFileInput openTypeReader;

		status = openTypeReader.ReadOpenTypeFile(otfFile.GetInputStream());
		if(status != ePDFSuccess)
		{
			cout<<"could not read open type file\n";
			break;
		}

		// show just abcd and notdef

		status = SaveCharstringCode(0,0,&openTypeReader.mCFF);
		for(unsigned short i=66; i < 70 && ePDFSuccess == status; ++i)
			status = SaveCharstringCode(0,i,&openTypeReader.mCFF);
	}while(false);
	return status;
}

ADD_CATEGORIZED_TEST(OpenTypeTest,"OpenType")
