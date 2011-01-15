#include "JPGImageTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFImageXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"

#include <iostream>

using namespace std;

JPGImageTest::JPGImageTest(void)
{
}

JPGImageTest::~JPGImageTest(void)
{
}

EStatusCode JPGImageTest::Run()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\BasicJPGImagesTest.PDF",ePDFVersion13);
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
		}

		// draw a rectangle
		pageContentContext->q();
		pageContentContext->k(100,0,0,0);
		pageContentContext->re(500,0,100,100);
		pageContentContext->f();
		pageContentContext->Q();

		// pause stream to start writing the image
		status = pdfWriter.PausePageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to pause page content context\n";
			break;
		}

		// Create image xobject from 
		PDFImageXObject* imageXObject  = pdfWriter.CreateImageXObjectFromJPGFile(L"C:\\PDFLibTests\\TestMaterials\\otherStage.JPG");
		if(!imageXObject)
		{
			wcout<<"failed to create image XObject from file\n";
			status = eFailure;
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
		if(status != eSuccess)
		{
			wcout<<"failed to pause page content context ||\n";
			break;
		}


		PDFFormXObject*  formXObject = pdfWriter.CreateFormXObjectFromJPGFile(L"C:\\PDFLibTests\\TestMaterials\\otherStage.JPG");
		if(!formXObject)
		{
			wcout<<"failed to create form XObject from file\n";
			status = eFailure;
			break;
		}

		// continue page drawing size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,400);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
		pageContentContext->Q();

		delete formXObject;

		status = pdfWriter.EndPageContentContext(pageContentContext);
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

ADD_CETEGORIZED_TEST(JPGImageTest,"PDF Images")
