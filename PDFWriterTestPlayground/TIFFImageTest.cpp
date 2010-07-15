#include "TIFFImageTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFImageXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

using namespace std;

TIFFImageTest::TIFFImageTest(void)
{
}

TIFFImageTest::~TIFFImageTest(void)
{
}

EStatusCode TIFFImageTest::Run()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{

		status = pdfWriter.InitializePDFWriter(L"C:\\PDFLibTests\\BasicTIFFImagesTest.PDF");
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

		PDFImageXObject* imageXObject = pdfWriter.CreateImageXObjectFromTIFFFile(L"C:\\PDFLibTests\\TestMaterials\\TiffSample1.TIF");
		if(!imageXObject)
		{
			wcout<<"failed to create image XObject from file\n";
			status = eFailure;
			break;
		}

		string imageXObjectName = page->GetResourcesDictionary().AddImageXObjectMapping(imageXObject->GetImageObjectID());

		// continue page drawing, place the image in 10,100, size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(500,0,0,400,10,100);
		pageContentContext->Do(imageXObjectName,imageXObject);
		pageContentContext->Q();

		delete imageXObject;

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

ADD_CETEGORIZED_TEST(TIFFImageTest,"PDF Images")
