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
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "Trace.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace PDFHummus;


static EStatusCode RunCFFTest(char* argv[],bool inEmbedFonts)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv, inEmbedFonts ? "SimpleTextUsageCFF.pdf" : "SimpleTextUsageCFFNoEmbed.pdf"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,
                                                     BuildRelativeOutputPath(argv,"SimpleTextUsage.log")),
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
                                                     BuildRelativeInputPath(
                                                                            argv,
                                                                            "fonts/BrushScriptStd.otf"));
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
                                                      BuildRelativeInputPath(
                                                                             argv,
                                                                             "fonts/KozGoPro-Regular.otf"));
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

static EStatusCode RunTrueTypeTest(char* argv[],bool inEmbedFonts)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(
			BuildRelativeOutputPath(argv, inEmbedFonts ? "SimpleTextUsageTrueType.pdf" : "SimpleTextUsageTrueTypeNoEmbed.pdf"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,BuildRelativeOutputPath(argv,"SimpleTextUsage.log")),
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
                                BuildRelativeInputPath(argv,"fonts/arial.ttf"));
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

static EStatusCode RunType1Test(char* argv[],bool inEmbedFonts)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(
			BuildRelativeOutputPath(argv, inEmbedFonts ? "SimpleTextUsageType1.pdf" : "SimpleTextUsageType1NoEmbed.pdf"),
                         ePDFVersion13,
                         LogConfiguration(true,true,BuildRelativeOutputPath(argv,"SimpleTextUsage.log")),
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
                                BuildRelativeInputPath(argv,"fonts/HLB_____.PFB"),
                                BuildRelativeInputPath(argv,"fonts/HLB_____.PFM"));
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

		EStatusCode encodingStatus = contentContext->Tj("abcd \xC3\xA1"); // ending char is LATIN SMALL LETTER A WITH ACUTE
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

static EStatusCode RunNoTextTest(char* argv[],bool inEmbedFonts)
{
    // this one checks an edge case where a font object is created but no text written. should not fail.
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
		status = pdfWriter.StartPDF(
			BuildRelativeOutputPath(argv, inEmbedFonts ? "SimpleNoTextUsage.pdf" : "SimpleNoTextUsageNoEmbed.pdf"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,BuildRelativeOutputPath(argv,"SimpleTextUsage.log")),
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
                                                     BuildRelativeInputPath(argv,"fonts/HLB_____.PFB"),
                                                     BuildRelativeInputPath(argv,"fonts/HLB_____.PFM"));
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

int SimpleTextUsage(int argc, char* argv[])
{
	EStatusCode status;

	// running two versions of the tests, one with font embedding, one without

	do
	{
		status = RunCFFTest(argv,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed CFF Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunCFFTest(argv, false);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed CFF Test without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunTrueTypeTest(argv,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed True Type Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunTrueTypeTest(argv,false);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed True Type Test, without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunType1Test(argv,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed Type 1 Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunType1Test(argv,false);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed Type 1 Test without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}


		status = RunNoTextTest(argv,true);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed No Text Test with font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}
        
		status = RunNoTextTest(argv,true);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Failed No Text Test without font embedding\n";
			status = PDFHummus::eFailure;
			break;
		}
	}while(false);

	return status == eSuccess? 0:1;
}

