#include "TiffSpecialsTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

using namespace std;

TiffSpecialsTest::TiffSpecialsTest(void)
{
}

TiffSpecialsTest::~TiffSpecialsTest(void)
{
}


EStatusCode TiffSpecialsTest::Run()
{
	PDFWriter pdfWriter;
	EStatusCode status; 
	TIFFUsageParameters TIFFParameters;

	do
	{

		status = pdfWriter.InitializePDFWriter(L"C:\\PDFLibTests\\TiffSpecialsTest.PDF",LogConfiguration(true,L"C:\\PDFLibTests\\TiffSpecialsTestLog.txt"));
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


		// multipage Tiff
		for(int i=0;i<4 && (eSuccess == status);++i)
		{
			TIFFParameters.PageIndex = i;
			PDFFormXObject* imageFormXObject = pdfWriter.CreateFormXObjectFromTIFFFile(L"C:\\PDFLibTests\\TestMaterials\\multipage.tif",TIFFParameters);
			if(!imageFormXObject)
			{
				wcout<<"failed to create image form XObject from file, for file "<<"C:\\PDFLibTests\\TestMaterials\\multipage.tif page "<<i<<"\n";
				status = eFailure;
			}
			else
				status = CreatePageForImageAndRelease(pdfWriter,imageFormXObject);
		}

		if(status != eSuccess)
			break;

		// Black and White mask
		status = CreateBlackAndWhiteMaskImage(pdfWriter);
		if(status != eSuccess)
			break;

		//Create BiLevel grayscales
		status = CreateBiLevelGrayScales(pdfWriter);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
}

EStatusCode TiffSpecialsTest::CreatePageForImageAndRelease(PDFWriter& inPDFWriter,PDFFormXObject* inImageObject)
{
	EStatusCode status = eSuccess;

	do
	{
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inPDFWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
		}


		string imageXObjectName = page->GetResourcesDictionary().AddFormXObjectMapping(inImageObject->GetObjectID());

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,0);
		pageContentContext->Do(imageXObjectName);
		pageContentContext->Q();

		delete inImageObject;

		status = inPDFWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context, for Image form xobject"<<inImageObject->GetObjectID()<<"\n";
			break;
		}

		status = inPDFWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page, for image form xobject "<<inImageObject->GetObjectID()<<"\n";
			break;
		}
	}while(false);

	return status;
}

static const wstring scJimBW = L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___ah.tif";

EStatusCode TiffSpecialsTest::CreateBlackAndWhiteMaskImage(PDFWriter& inPDFWriter)
{
	EStatusCode status = eSuccess;
	TIFFUsageParameters TIFFParameters;

	do
	{
		PDFFormXObject* imageBW = inPDFWriter.CreateFormXObjectFromTIFFFile(scJimBW);
		if(!imageBW)
		{
			wcout<<"failed to create image BW, for file "<<scJimBW<<"\n";
			status = eFailure;
			break;
		}

		TIFFParameters.BWTreatment.AsImageMask = true;
		TIFFParameters.BWTreatment.OneColor = CMYKRGBColor(255,128,0);
		PDFFormXObject* imageBWMask = inPDFWriter.CreateFormXObjectFromTIFFFile(scJimBW,TIFFParameters);
		if(!imageBWMask)
		{
			wcout<<"failed to create image mask BW, for file "<<scJimBW<<"\n";
			status = eFailure;
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inPDFWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
		}

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,842 - 195.12);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageBW->GetObjectID()));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,159.36,842 - 195.12);
		pageContentContext->rg(0,0,1);
		pageContentContext->re(0,0,159.36,195.12);
		pageContentContext->f();
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageBWMask->GetObjectID()));
		pageContentContext->Q();

		delete imageBW;
		delete imageBWMask;

		status = inPDFWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context, for Image BWs test\n";
			break;
		}

		status = inPDFWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page, for image BWs test\n";
			break;
		}
	}while(false);

	return status;
}

static const wstring scWJim = L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___cg.tif";

EStatusCode TiffSpecialsTest::CreateBiLevelGrayScales(PDFWriter& inPDFWriter)
{
	EStatusCode status = eSuccess;
	TIFFUsageParameters TIFFParameters;

	do
	{

		// GrayScale regular
		PDFFormXObject* imageGrayScale = inPDFWriter.CreateFormXObjectFromTIFFFile(scWJim);
		if(!imageGrayScale)
		{
			wcout<<"failed to create image form XObject from file, grayscale for file "<<scWJim;
			status = eFailure;
			break;
		}

		// GrayScale Green
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(0,255,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(255,255,255);
		PDFFormXObject* imageGrayScaleGreen = inPDFWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleGreen)
		{
			wcout<<"failed to create image form XObject from file, green n white for file "<<scWJim;
			status = eFailure;
			break;
		}


		// grayscale cyan magenta
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(255,255,0,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0,0,0,0);
		PDFFormXObject* imageGrayScaleCyanMagenta = inPDFWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleCyanMagenta)
		{
			wcout<<"failed to create image form XObject from file, cyan magenta n white for file "<<scWJim;
			status = eFailure;
			break;
		}

		// grayscale green vs red
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(0,255,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(255,0,0);
		PDFFormXObject* imageGrayScaleGreenVSRed = inPDFWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleGreenVSRed)
		{
			wcout<<"failed to create image form XObject from file, green n red for file "<<scWJim;
			status = eFailure;
			break;
		}

		// grayscale cyan vs magenta
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(255,0,0,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0,255,0,0);
		PDFFormXObject* imageGrayScaleCyanVSMagenta = inPDFWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleCyanVSMagenta)
		{
			wcout<<"failed to create image form XObject from file, cyan n magenta for file "<<scWJim;
			status = eFailure;
			break;
		}

		// start page
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inPDFWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
		}

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,842 - 195.12);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageGrayScale->GetObjectID()));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,159.36,842 - 195.12);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleGreen->GetObjectID()));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,159.36*2,842 - 195.12);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleCyanMagenta->GetObjectID()));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,842 - 195.12*2);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleGreenVSRed->GetObjectID()));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,159.36,842 - 195.12*2);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleCyanVSMagenta->GetObjectID()));
		pageContentContext->Q();

		delete imageGrayScale;
		delete imageGrayScaleGreen;
		delete imageGrayScaleCyanMagenta;
		delete imageGrayScaleGreenVSRed;
		delete imageGrayScaleCyanVSMagenta;

		status = inPDFWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context, for bilevel grayscale images\n";
			break;
		}

		status = inPDFWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page, for bilevel grayscale images\n";
			break;
		}
	}while(false);

	return status;
}

ADD_CETEGORIZED_TEST(TiffSpecialsTest,"PDF Images")