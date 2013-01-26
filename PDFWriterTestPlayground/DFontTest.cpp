/*
 Source File : DFontTest.cpp
 
 
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

#include "DFontTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

using namespace PDFHummus;

DFontTest::DFontTest()
{
    
}

DFontTest::~DFontTest()
{
    
}


EStatusCode DFontTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
		status = pdfWriter.StartPDF(
                                    RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"DFontTest.PDF"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"DFontTest.log")));
		if(status != eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}
        
        PDFUsedFont* courierFonts[4];
        
        for(int i=0; i < 4 && eSuccess == status; ++i)
        {
            courierFonts[i] = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/courier.dfont"),i);
            if(!courierFonts[i])
            {
                status = eFailure;
                cout<<"Failed to create font object for lucida Grande font at "<<i<<"\n";
                break;
            }
        
        }
        if(status != eSuccess)
            break;
 
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = eFailure;
			cout<<"failed to create content context for page\n";
			break;
		}
        

        for(int i=0;i<4;++i)
        {
            contentContext->BT();
            contentContext->k(0,0,0,1);
            contentContext->Tf(courierFonts[i],1);
            contentContext->Tm(30,0,0,30,78.4252,662.8997 - i*100);
            EStatusCode encodingStatus = contentContext->Tj("Hello World!");
            if(encodingStatus != eSuccess)
                cout<<"Could not find some of the glyphs for this font";
            contentContext->ET();
        }
        
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
		{
			cout<<"failed to end page content context\n";
			break;
		}
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			cout<<"failed to write page\n";
			break;
		}
        
		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
}


ADD_CATEGORIZED_TEST(DFontTest,"Font Packages")

