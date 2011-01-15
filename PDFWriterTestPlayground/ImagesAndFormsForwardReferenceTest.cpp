#include "ImagesAndFormsForwardReferenceTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFImageXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"
#include "ProcsetResourcesConstants.h"
#include "ObjectsContext.h"
#include "IndirectObjectsReferenceRegistry.h"
#include "xobjectContentContext.h"

#include <iostream>

using namespace std;

ImagesAndFormsForwardReferenceTest::ImagesAndFormsForwardReferenceTest(void)
{
}

ImagesAndFormsForwardReferenceTest::~ImagesAndFormsForwardReferenceTest(void)
{
}

EStatusCode ImagesAndFormsForwardReferenceTest::Run()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{

		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\ImagesAndFormsForwardReferenceTest.PDF",ePDFVersion13);
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



		// continue page drawing size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(500,0,0,400,0,0);
		
		ObjectIDType imageXObjectID = pdfWriter.GetObjectsContext().GetInDirectObjectsRegistry().AllocateNewObjectID();
		pageContentContext->Do(page->GetResourcesDictionary().AddImageXObjectMapping(imageXObjectID));

		// optionally i can also add the necessary PDF Procsets. i'll just add all that might be relevant
		page->GetResourcesDictionary().AddProcsetResource(KProcsetImageB);
		page->GetResourcesDictionary().AddProcsetResource(KProcsetImageC);
		page->GetResourcesDictionary().AddProcsetResource(KProcsetImageI);

		pageContentContext->Q();

		// continue page drawing size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,400);
		ObjectIDType formXObjectID = pdfWriter.GetObjectsContext().GetInDirectObjectsRegistry().AllocateNewObjectID();
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObjectID));
		pageContentContext->Q();

		pageContentContext->q();
		ObjectIDType tiffFormXObjectID = pdfWriter.GetObjectsContext().GetInDirectObjectsRegistry().AllocateNewObjectID();
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(tiffFormXObjectID));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,100,500);
		ObjectIDType simpleFormXObjectID = pdfWriter.GetObjectsContext().GetInDirectObjectsRegistry().AllocateNewObjectID();
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(simpleFormXObjectID));
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


		// Create image xobject  
		PDFImageXObject* imageXObject  = pdfWriter.CreateImageXObjectFromJPGFile(L"C:\\PDFLibTests\\TestMaterials\\otherStage.JPG",imageXObjectID);
		if(!imageXObject)
		{
			wcout<<"failed to create image XObject from file\n";
			status = eFailure;
			break;
		}

		// now create form xobject
		PDFFormXObject*  formXObject = pdfWriter.CreateFormXObjectFromJPGFile(L"C:\\PDFLibTests\\TestMaterials\\otherStage.JPG",formXObjectID);
		if(!formXObject)
		{
			wcout<<"failed to create form XObject from file\n";
			status = eFailure;
			break;
		}

		PDFFormXObject* tiffFormXObject = pdfWriter.CreateFormXObjectFromTIFFFile(L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___ah.tif",tiffFormXObjectID);
		if(!tiffFormXObject)
		{
			wcout<<"failed to create image form XObject from file, for file\n";
			status = eFailure;
			break;
		}


		delete imageXObject;
		delete formXObject;
		delete tiffFormXObject;


		// define an xobject form to draw a 200X100 points red rectangle
		PDFFormXObject* xobjectForm = pdfWriter.StartFormXObject(PDFRectangle(0,0,200,100),simpleFormXObjectID);

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


		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;		
}

ADD_CETEGORIZED_TEST(ImagesAndFormsForwardReferenceTest,"PDF Images")
