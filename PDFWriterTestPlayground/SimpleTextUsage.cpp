/*
   Source File : SimpleTextUsage.cpp


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
#include "SimpleTextUsage.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "TestsRunner.h"

#include <iostream>

using namespace PDFHummus;

SimpleTextUsage::SimpleTextUsage(void)
{
}

SimpleTextUsage::~SimpleTextUsage(void)
{
}



#include "Trace.h"
EStatusCode SimpleTextUsage::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;

	// running two versions of the tests, one with font embedding, one without

	do
	{
		status = RunCFFTest(inTestConfiguration,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed CFF Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunCFFTest(inTestConfiguration, false);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed CFF Test without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunTrueTypeTest(inTestConfiguration,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed True Type Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunTrueTypeTest(inTestConfiguration,false);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed True Type Test, without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunType1Test(inTestConfiguration,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed Type 1 Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunType1Test(inTestConfiguration,false);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed Type 1 Test without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}


		status = RunNoTextTest(inTestConfiguration,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed No Text Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}
        
		status = RunNoTextTest(inTestConfiguration,true);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed No Text Test without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}
	}while(false);

	return status;
}

EStatusCode SimpleTextUsage::RunCFFTest(const TestConfiguration& inTestConfiguration,bool inEmbedFonts)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, inEmbedFonts ? "SimpleTextUsageCFF.PDF" : "SimpleTextUsageCFFNoEmbed.PDF"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,
                                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleTextUsage.log")),
													 PDFCreationSettings(true, inEmbedFonts));
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
                                                     RelativeURLToLocalPath(
                                                                            inTestConfiguration.mSampleFileBase,
                                                                            "TestMaterials/fonts/BrushScriptStd.otf"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for brushscriptstd.otf\n";
			break;
		}

		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj("abcd");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font (BrushScriptStd)";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		// now write some more text with kozuka font
		contentContext->BT();
		contentContext->k(0,0,0,1);

		PDFUsedFont* fontK = pdfWriter.GetFontForFile(
                                                      RelativeURLToLocalPath(
                                                                             inTestConfiguration.mSampleFileBase,
                                                                             "TestMaterials/fonts/KozGoPro-Regular.otf"));
		if(!fontK)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for KozGoPro-Regular.otf\n";
			break;
		}

		contentContext->Tf(fontK,1);

		contentContext->Tm(30,0,0,30,78.4252,400.8997);

		encodingStatus = contentContext->Tj("abcd");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font (Kozuka)";

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

EStatusCode SimpleTextUsage::RunTrueTypeTest(const TestConfiguration& inTestConfiguration,bool inEmbedFonts)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, inEmbedFonts ? "SimpleTextUsageTrueType.PDF" : "SimpleTextUsageTrueTypeNoEmbed.PDF"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleTextUsage.log")),
									PDFCreationSettings(true,inEmbedFonts));
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
                                RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for arial.ttf\n";
			break;
		}


		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj("hello world");
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

EStatusCode SimpleTextUsage::RunType1Test(const TestConfiguration& inTestConfiguration,bool inEmbedFonts)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, inEmbedFonts ? "SimpleTextUsageType1.PDF" : "SimpleTextUsageType1NoEmbed.PDF"),
                         ePDFVersion13,
                         LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleTextUsage.log")),
						 PDFCreationSettings(true,inEmbedFonts));
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
                                RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFB"),
                                RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFM"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for Helvetica Neue font\n";
			break;
		}


		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj("abcd");
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

EStatusCode SimpleTextUsage::RunNoTextTest(const TestConfiguration& inTestConfiguration,bool inEmbedFonts)
{
    // this one checks an edge case where a font object is created but no text written. should not fail.
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
		status = pdfWriter.StartPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, inEmbedFonts ? "SimpleNoTextUsage.PDF" : "SimpleNoTextUsageNoEmbed.PDF"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleTextUsage.log")),
									PDFCreationSettings(true,inEmbedFonts));
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
                                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFB"),
                                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/HLB_____.PFM"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for Helvetica Neue font\n";
			break;
		}
        
        
		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);
        
		contentContext->Tf(font,1);
        
		contentContext->Tm(30,0,0,30,78.4252,662.8997);

        // no text is written!!!
        
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


ADD_CATEGORIZED_TEST(SimpleTextUsage,"PDF")
