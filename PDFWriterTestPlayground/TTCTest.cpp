/*
 Source File : TTCTest.cpp
 
 
 Copyright 2013 Gal Kahana PDFWriter
 
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

#include "TTCTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

using namespace PDFHummus;

TTCTest::TTCTest()
{
    
}

TTCTest::~TTCTest()
{
    
}


PDFHummus::EStatusCode TTCTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
		status = pdfWriter.StartPDF(
                                    RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TTCTestLucidaGrande.PDF"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TTCTestLucidaGrande.log")));
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
        
		PDFUsedFont* fontLucidaGrande0 = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/LucidaGrande.ttc"),0);
		if(!fontLucidaGrande0)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for lucida Grande font at 0\n";
			break;
		}
        
        PDFUsedFont* fontLucidaGrande1 = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/LucidaGrande.ttc"),1);
		if(!fontLucidaGrande1)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for lucida Grande font at 1\n";
			break;
		}
        
		// Draw some text in Lucida Grande 0 (regular)
		contentContext->BT();
		contentContext->k(0,0,0,1);
		contentContext->Tf(fontLucidaGrande0,1);
		contentContext->Tm(30,0,0,30,78.4252,662.8997);
		EStatusCode encodingStatus = contentContext->Tj("Hello World!");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font";
		contentContext->ET();

        // Draw some text in Lucida Grande 0 (bold)
		contentContext->BT();
		contentContext->k(0,0,0,1);
		contentContext->Tf(fontLucidaGrande1,1);
		contentContext->Tm(30,0,0,30,78.4252,462.8997);
		encodingStatus = contentContext->Tj("Hello World!");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font";
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


ADD_CATEGORIZED_TEST(TTCTest,"Font Packages")

