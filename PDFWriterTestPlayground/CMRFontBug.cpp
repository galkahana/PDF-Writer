/*
 Source File : CMRFontBugTest.cpp
 
 
 Copyright 2012 Gal Kahana PDFWriter
 
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
#include "CMRFontBug.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "TestsRunner.h"
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "OpenTypeFileInput.h"
#include "OutputFile.h"
#include "OutputStreamTraits.h"
#include "CharStringType2Tracer.h"
#include "CFFFileInput.h"
#include "BoxingBase.h"
#include "PDFStreamInput.h"

#include <iostream>

using namespace PDFHummus;

CMRFontBugTest::CMRFontBugTest(void)
{
}

CMRFontBugTest::~CMRFontBugTest()
{
}

EStatusCode CMRFontBugTest::Run(const TestConfiguration& inTestConfiguration)
{
    string filePath = RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"CMRFontBugTest.PDF");
    
    EStatusCode status = CreatePDFFileWithFont(inTestConfiguration,filePath);
    
    do {
        if(status != eSuccess)
            break;
        
       // status = InspectCreatedFontGlyphs(inTestConfiguration,filePath);
    } while (false);
    
    return status;
}


EStatusCode CMRFontBugTest::CreatePDFFileWithFont(const TestConfiguration& inTestConfiguration,const std::string& inOutputFilePath)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
 		status = pdfWriter.StartPDF(
                                    inOutputFilePath,
                                    ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
			break;
		}
        
		PDFUsedFont* font = pdfWriter.GetFontForFile(
                                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/cmr10.pfb"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for cmr10.pfb\n";
			break;
		}
        
        
		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);
        
		contentContext->Tf(font,1);
        
		contentContext->Tm(30,0,0,30,78.4252,662.8997);
        
        std::string buf;
        buf.push_back(0);
        buf.push_back(1);
        buf.push_back(2);
        buf.push_back(3);
        buf.push_back(4);
        buf.push_back(5);
        buf.push_back(6);
              
		EStatusCode encodingStatus = contentContext->Tj(buf);
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font";
        
		// continue even if failed...want to see how it looks like
		contentContext->ET();
        
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context\n";
			break;
		}
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page\n";
			break;
		}
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
    
}

PDFHummus::EStatusCode CMRFontBugTest::InspectCreatedFontGlyphs(const TestConfiguration& inTestConfiguration,const std::string& inPDFFilePath)
{
    EStatusCode status = eSuccess;
    InputFile pdfFile;
    
    do
    {
        pdfFile.OpenFile(inPDFFilePath);
        
        PDFParser parser;
        
        parser.StartPDFParsing(pdfFile.GetInputStream());
        
        
        PDFObjectCastPtr<PDFStreamInput> fontFile(parser.ParseNewObject(6));
        
        IByteReader* reader = parser.StartReadingFromStream(fontFile.GetPtr());
   
        
        OutputFile outputFile;
        
        outputFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"temp.txt"));
        
        OutputStreamTraits traits(outputFile.GetOutputStream());
        
        traits.CopyToOutputStream(reader);
        
        outputFile.CloseFile();
        
        
        CFFFileInput openTypeReader;
        InputFile tempFile;
        
        tempFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"temp.txt"));
        
        openTypeReader.ReadCFFFile(tempFile.GetInputStream());
        
        for(unsigned short i=0; i < openTypeReader.GetCharStringsCount(0) && eSuccess == status;++i)
            status = SaveCharstringCode(inTestConfiguration,0,i,&openTypeReader);
        
        
        
    } while (false);
    
    return status;
}

EStatusCode CMRFontBugTest::SaveCharstringCode(const TestConfiguration& inTestConfiguration,unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput)
{
	OutputFile glyphFile;
    
	EStatusCode status = glyphFile.OpenFile(
                                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,
                                                                   string("glyphCFF")  + Long(inFontIndex).ToString() + "_" + inCFFFileInput->GetGlyphName(0,inGlyphIndex) + ".txt"));
    
	do
	{
		if(status != PDFHummus::eSuccess)
			break;
		
		CharStringType2Tracer tracer;
        
		status = tracer.TraceGlyphProgram(inFontIndex,inGlyphIndex,inCFFFileInput,glyphFile.GetOutputStream());
        
	}while(false);
    
	glyphFile.CloseFile();
    
	return status;
}

ADD_CATEGORIZED_TEST(CMRFontBugTest,"PDF")
