/*
   Source File : LinksTest.cpp


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
#include "LinksTest.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"
#include "TestsRunner.h"

#include <iostream>
using namespace PDFHummus;

LinksTest::LinksTest(void)
{
}

LinksTest::~LinksTest(void)
{
}

EStatusCode LinksTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"LinksTest.PDF"),ePDFVersion13,LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"LinksTest.log")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFFormXObject* soundCloudLogo = pdfWriter.CreateFormXObjectFromJPGFile(
                                                RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/Images/soundcloud_logo.jpg"));


		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
			break;
		}

		PDFUsedFont* font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
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
		contentContext->Tm(11,0,0,11,90.024,709.54);
		EStatusCode encodingStatus = contentContext->Tj("http://pdfhummus.com");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font (arial)";
		// continue even if failed...want to see how it looks like
		contentContext->ET();

		// Draw soundcloud loog
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,90.024,200);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(soundCloudLogo->GetObjectID()));
		contentContext->Q();
		
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context\n";
			break;
		}

		delete soundCloudLogo;


		// now let's attach some links.

		// first, the link for the test:
		pdfWriter.AttachURLLinktoCurrentPage("http://www.pdfhummus.com",PDFRectangle(87.75,694.56,198.76,720));

		// second, link for the logo.
		pdfWriter.AttachURLLinktoCurrentPage("http://www.soundcloud.com",PDFRectangle(90.024,200,367.524,375));

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


ADD_CATEGORIZED_TEST(LinksTest,"PDF")