/*
   Source File : JPGImageTest.cpp


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
#include "JPGImageTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFImageXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

JPGImageTest::JPGImageTest(void)
{
}

JPGImageTest::~JPGImageTest(void)
{
}

EStatusCode JPGImageTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"BasicJPGImagesTest.PDF"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
		}

		// draw a rectangle
		pageContentContext->q();
		pageContentContext->k(100,0,0,0);
		pageContentContext->re(500,0,100,100);
		pageContentContext->f();
		pageContentContext->Q();

		// pause stream to start writing the image
		status = pdfWriter.PausePageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to pause page content context\n";
			break;
		}

		// Create image xobject from 
		PDFImageXObject* imageXObject  = pdfWriter.CreateImageXObjectFromJPGFile(
                                                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/otherStage.JPG"));
		if(!imageXObject)
		{
			cout<<"failed to create image XObject from file\n";
			status = PDFHummus::eFailure;
			break;
		}

		// continue page drawing size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(500,0,0,400,0,0);
		pageContentContext->Do(page->GetResourcesDictionary().AddImageXObjectMapping(imageXObject));
		pageContentContext->Q();

		delete imageXObject;

		// now do the same with a form xobject
		// pause stream to start writing the image
		status = pdfWriter.PausePageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to pause page content context ||\n";
			break;
		}


		PDFFormXObject*  formXObject = pdfWriter.CreateFormXObjectFromJPGFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/otherStage.JPG"));
		if(!formXObject)
		{
			cout<<"failed to create form XObject from file\n";
			status = PDFHummus::eFailure;
			break;
		}

		// continue page drawing size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,400);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
		pageContentContext->Q();

		delete formXObject;

		status = pdfWriter.EndPageContentContext(pageContentContext);
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

ADD_CATEGORIZED_TEST(JPGImageTest,"PDF Images")
