/*
   Source File : SimpleContentPageTest.cpp


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
#include "SimpleContentPageTest.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

SimpleContentPageTest::SimpleContentPageTest(void)
{
}

SimpleContentPageTest::~SimpleContentPageTest(void)
{
}

/*
	Creates a single page PDF with some drawings
*/

EStatusCode SimpleContentPageTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleContent.PDF"),ePDFVersion13);
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

		// draw a 100X100 points cyan square
		contentContext->q();
		contentContext->k(100,0,0,0);
		contentContext->re(100,500,100,100);
		contentContext->f();
		contentContext->Q();

		// force stream change
		status = pdfWriter.PausePageContentContext(contentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to pause page content context\n";
			break;
		}

		// draw a 200X100 points red rectangle
		contentContext->q();
		contentContext->k(0,100,100,0);
		contentContext->re(200,600,200,100);
		contentContext->f();
		contentContext->Q();

		// draw a gray line
		contentContext->q();
		contentContext->G(0.5);
		contentContext->w(3);
		contentContext->m(200,600);
		contentContext->l(400,400);
		contentContext->S();
		contentContext->Q();
		
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

ADD_CATEGORIZED_TEST(SimpleContentPageTest,"PDF")
