/*
   Source File : ShutDownRestartTest.cpp


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
#include "ShutDownRestartTest.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"
#include "XObjectContentContext.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

ShutDownRestartTest::ShutDownRestartTest(void)
{
}

ShutDownRestartTest::~ShutDownRestartTest(void)
{
}


EStatusCode ShutDownRestartTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status; 

	do
	{
		{
			PDFWriter pdfWriterA;
			status = pdfWriterA.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleContentShutdownRestart.PDF"),ePDFVersion13);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to start PDF\n";
				break;
			}	

			PDFPage* page = new PDFPage();
			page->SetMediaBox(PDFRectangle(0,0,595,842));

			PDFUsedFont* font = pdfWriterA.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
			if(!font)
			{
				status = PDFHummus::eFailure;
				cout<<"Failed to create font object for arial.ttf\n";
				break;
			}

			PageContentContext* contentContext = pdfWriterA.StartPageContentContext(page);
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
			status = pdfWriterA.PausePageContentContext(contentContext);
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
				
			status = pdfWriterA.EndPageContentContext(contentContext);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to end page content context\n";
				break;
			}

			status = pdfWriterA.WritePageAndRelease(page);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to write page\n";
				break;
			}


			status = pdfWriterA.Shutdown(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"ShutDownRestartState.txt"));
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to shutdown library\n";
				break;
			}

		}
		{
			PDFWriter pdfWriterB;
			status = pdfWriterB.ContinuePDF(
                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"SimpleContentShutdownRestart.PDF"),
                            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"ShutDownRestartState.txt")
                    );
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to restart library\n";
				break;
			}	

			PDFPage* page = new PDFPage();
			page->SetMediaBox(PDFRectangle(0,0,595,842));

			PageContentContext* pageContentContext = pdfWriterB.StartPageContentContext(page);
			if(NULL == pageContentContext)
			{
				status = PDFHummus::eFailure;
				cout<<"failed to create content context for page\n";
			}

			// draw a 100X100 points cyan square
			pageContentContext->q();
			pageContentContext->k(100,0,0,0);
			pageContentContext->re(100,500,100,100);
			pageContentContext->f();
			pageContentContext->Q();

			// pause stream to start writing a form xobject
			status = pdfWriterB.PausePageContentContext(pageContentContext);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to pause page content context\n";
				break;
			}

			// define an xobject form to draw a 200X100 points red rectangle
			PDFFormXObject* xobjectForm = pdfWriterB.StartFormXObject(PDFRectangle(0,0,200,100));
			ObjectIDType formObjectID = xobjectForm->GetObjectID();

			XObjectContentContext* xobjectContentContext = xobjectForm->GetContentContext();
			xobjectContentContext->q();
			xobjectContentContext->k(0,100,100,0);
			xobjectContentContext->re(0,0,200,100);
			xobjectContentContext->f();
			xobjectContentContext->Q();

			status = pdfWriterB.EndFormXObjectAndRelease(xobjectForm);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to write XObject form\n";
				break;
			}

			string formXObjectName = page->GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

			// continue page drawing, place the form in 200,600
			pageContentContext->q();
			pageContentContext->cm(1,0,0,1,200,600);
			pageContentContext->Do(formXObjectName);
			pageContentContext->Q();

			// draw a gray line
			pageContentContext->q();
			pageContentContext->G(0.5);
			pageContentContext->w(3);
			pageContentContext->m(200,600);
			pageContentContext->l(400,400);
			pageContentContext->S();
			pageContentContext->Q();

			// place the form in another location
			pageContentContext->q();
			pageContentContext->cm(1,0,0,1,200,200);
			pageContentContext->Do(formXObjectName);
			pageContentContext->Q();

			status = pdfWriterB.EndPageContentContext(pageContentContext);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to end page content context\n";
				break;
			}

			status = pdfWriterB.WritePageAndRelease(page);
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed to write page\n";
				break;
			}

			status = pdfWriterB.EndPDF();
			if(status != PDFHummus::eSuccess)
			{
				cout<<"failed in end PDF\n";
				break;
			}
		}
	}while(false);
	return status;

}

ADD_CATEGORIZED_TEST(ShutDownRestartTest,"PDF")