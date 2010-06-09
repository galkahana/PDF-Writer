#include "FormXObjectTest.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"
#include "XObjectContentContext.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;

FormXObjectTest::FormXObjectTest(void)
{
}

FormXObjectTest::~FormXObjectTest(void)
{
}

EStatusCode FormXObjectTest::Run()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{

		//status = pdfWriter.InitializePDFWriter(L"C:\\PDFLibTests\\XObjectContent.PDF",LogConfiguration::DefaultLogConfiguration,PDFCreationSettings(false));
		status = pdfWriter.InitializePDFWriter(L"C:\\PDFLibTests\\XObjectContent.PDF");
		if(status != eSuccess)
		{
			wcout<<"failed to initialize PDF Writer\n";
			break;
		}

		status = pdfWriter.StartPDF(ePDFVersion13);
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

		// draw a 100X100 points cyan square
		pageContentContext->q();
		pageContentContext->k(100,0,0,0);
		pageContentContext->re(100,500,100,100);
		pageContentContext->f();
		pageContentContext->Q();

		// pause stream to start writing a form xobject
		status = pdfWriter.PausePageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to pause page content context\n";
			break;
		}

		// define an xobject form to draw a 200X100 points red rectangle
		PDFFormXObject* xobjectForm = pdfWriter.StartFormXObject(PDFRectangle(0,0,200,100));
		ObjectIDType formObjectID = xobjectForm->GetObjectID();

		XObjectContentContext* xobjectContentContext = xobjectForm->GetContentContext();
		xobjectContentContext->q();
		xobjectContentContext->k(0,100,100,0);
		xobjectContentContext->re(0,0,200,100);
		xobjectContentContext->f();
		xobjectContentContext->Q();

		status = pdfWriter.EndFormXObjectAndRelease(xobjectForm);
		if(status != eSuccess)
		{
			wcout<<"failed to write XObject form\n";
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
	
		// 2nd page, just uses the form from the previous page
		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for 2nd page\n";
		}
		
		formXObjectName = page->GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

		// place the form in 300,500
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,300,500);
		pageContentContext->Do(formXObjectName);
		pageContentContext->Q();

		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end 2nd page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write 2nd page\n";
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

ADD_CETEGORIZED_TEST(FormXObjectTest,"PDF")