#include "InputImagesAsStreamsTest.h"
#include "TestsRunner.h"
#include "InputFile.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"

#include <string>
#include <iostream>
using namespace std;
using namespace PDFHummus;

InputImagesAsStreamsTest::InputImagesAsStreamsTest(void)
{
}

InputImagesAsStreamsTest::~InputImagesAsStreamsTest(void)
{
}


EStatusCode InputImagesAsStreamsTest::Run(const TestConfiguration& inTestConfiguration)
{
	// A minimal test to see if images as streams work. i'm using regular file streams, just to show the point
	// obviously this is quite a trivial case.

	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"ImagesInStreams.PDF"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		// JPG image

		InputFile jpgImage;

		status = jpgImage.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/otherStage.JPG"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to open JPG image in"<<"TestMaterials/images/otherStage.JPG"<<"\n";
			break;
		}


		PDFFormXObject*  formXObject = pdfWriter.CreateFormXObjectFromJPGStream(jpgImage.GetInputStream());
		if(!formXObject)
		{
			cout<<"failed to create form XObject from file\n";
			status = PDFHummus::eFailure;
			break;
		}

		jpgImage.CloseFile();

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
		}

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

		// TIFF image
#ifndef PDFHUMMUS_NO_TIFF
		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		InputFile tiffFile;
		status = tiffFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/FLAG_T24.TIF"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to open TIFF image in"<<"TestMaterials/images/tiff/FLAG_T24.TIF"<<"\n";
			break;
	
        }
		formXObject = pdfWriter.CreateFormXObjectFromTIFFStream(tiffFile.GetInputStream());
		if(!formXObject)
		{
			cout<<"failed to create image form XObject for TIFF\n";
			status = PDFHummus::eFailure;
			break;
		}

		tiffFile.CloseFile();
		pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page with TIFF image\n";
		}

		// continue page drawing, place the image in 0,0 (playing...could avoid CM at all)
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,0);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
		pageContentContext->Q();

		delete formXObject;

		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context for TIFF\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page, for TIFF\n";
			break;
		}
#endif

		// PDF
		
		InputFile pdfFile;

		status = pdfFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/Original.pdf"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to open PDF file in"<<"TestMaterials/Original.pdf"<<"\n";
			break;
		}

		status = pdfWriter.AppendPDFPagesFromPDF(pdfFile.GetInputStream(),PDFPageRange()).first;
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from Original.PDF\n";
			break;
		}

		pdfFile.CloseFile();

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;	
}


ADD_CATEGORIZED_TEST(InputImagesAsStreamsTest,"CustomStreams")