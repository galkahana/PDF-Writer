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

SimpleTextUsage::SimpleTextUsage(void)
{
}

SimpleTextUsage::~SimpleTextUsage(void)
{
}



#include "Trace.h"
EStatusCode SimpleTextUsage::Run()
{
	EStatusCode status;

	do
	{
		status = RunCFFTest();
		if(status != eSuccess)
		{
			wcout<<"Failed CFF Test\n";
			status = eFailure;
			break;
		}

		status = RunTrueTypeTest();
		if(status != eSuccess)
		{
			wcout<<"Failed True Type Test\n";
			status = eFailure;
			break;
		}

		status = RunType1Test();
		if(status != eSuccess)
		{
			wcout<<"Failed Type 1 Test\n";
			status = eFailure;
			break;
		}

	}while(false);

	return status;
}

EStatusCode SimpleTextUsage::RunCFFTest()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\SimpleTextUsageCFF.PDF",ePDFVersion13,LogConfiguration(true,L"C:\\PDFLibTests\\SimpleTextUsage.log"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
			break;
		}

		PDFUsedFont* font = pdfWriter.GetFontForFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\BrushScriptStd.otf");
		if(!font)
		{
			status = eFailure;
			wcout<<"Failed to create font object for brushscriptstd.otf\n";
			break;
		}

		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj(L"abcd");
		if(encodingStatus != eSuccess)
			wcout<<"Could not find some of the glyphs for this font (BrushScriptStd)";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		// now write some more text with kozuka font
		contentContext->BT();
		contentContext->k(0,0,0,1);

		PDFUsedFont* fontK = pdfWriter.GetFontForFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\KozGoPro-Regular.otf");
		if(!fontK)
		{
			status = eFailure;
			wcout<<"Failed to create font object for KozGoPro-Regular.otf\n";
			break;
		}

		contentContext->Tf(fontK,1);

		contentContext->Tm(30,0,0,30,78.4252,400.8997);

		encodingStatus = contentContext->Tj(L"abcd");
		if(encodingStatus != eSuccess)
			wcout<<"Could not find some of the glyphs for this font (Kozuka)";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;	

}

EStatusCode SimpleTextUsage::RunTrueTypeTest()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\SimpleTextUsageTrueType.PDF",ePDFVersion13,LogConfiguration(true,L"C:\\PDFLibTests\\SimpleTextUsage.log"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
			break;
		}

		PDFUsedFont* font = pdfWriter.GetFontForFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\arial.ttf");
		if(!font)
		{
			status = eFailure;
			wcout<<"Failed to create font object for arial.ttf\n";
			break;
		}


		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj(L"hello world");
		if(encodingStatus != eSuccess)
			wcout<<"Could not find some of the glyphs for this font";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;	
}

EStatusCode SimpleTextUsage::RunType1Test()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\SimpleTextUsageType1.PDF",ePDFVersion13,LogConfiguration(true,L"C:\\PDFLibTests\\SimpleTextUsage.log"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
			break;
		}

		PDFUsedFont* font = pdfWriter.GetFontForFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFB",
													 L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFM");
		if(!font)
		{
			status = eFailure;
			wcout<<"Failed to create font object for Helvetica Neue font\n";
			break;
		}


		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj(L"abcd");
		if(encodingStatus != eSuccess)
			wcout<<"Could not find some of the glyphs for this font";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;		
}

ADD_CATEGORIZED_TEST(SimpleTextUsage,"PDF")
