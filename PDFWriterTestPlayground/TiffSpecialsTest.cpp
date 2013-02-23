/*
   Source File : TiffSpecialsTest.cpp


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

#ifndef PDFHUMMUS_NO_TIFF

#include "TiffSpecialsTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

TiffSpecialsTest::TiffSpecialsTest(void)
{
}

TiffSpecialsTest::~TiffSpecialsTest(void)
{
}


EStatusCode TiffSpecialsTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status; 
	TIFFUsageParameters TIFFParameters;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TiffSpecialsTest.PDF"),ePDFVersion13,LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TiffSpecialsTestLog.txt")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	


		// multipage Tiff
		for(int i=0;i<4 && (PDFHummus::eSuccess == status);++i)
		{
			TIFFParameters.PageIndex = i;
			PDFFormXObject* imageFormXObject = pdfWriter.CreateFormXObjectFromTIFFFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/multipage.tif"),TIFFParameters);
			if(!imageFormXObject)
			{
				cout<<"failed to create image form XObject from file, for file "<<"multipage.tif page "<<i<<"\n";
				status = PDFHummus::eFailure;
			}
			else
				status = CreatePageForImageAndRelease(pdfWriter,imageFormXObject);
		}

		if(status != PDFHummus::eSuccess)
			break;

		// Black and White mask
		status = CreateBlackAndWhiteMaskImage(inTestConfiguration,pdfWriter);
		if(status != PDFHummus::eSuccess)
			break;

		//Create BiLevel grayscales
		status = CreateBiLevelGrayScales(inTestConfiguration,pdfWriter);
		if(status != PDFHummus::eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
}

EStatusCode TiffSpecialsTest::CreatePageForImageAndRelease(PDFWriter& inpdfWriter,PDFFormXObject* inImageObject)
{
	EStatusCode status = PDFHummus::eSuccess;

	do
	{
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inpdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
		}


		string imageXObjectName = page->GetResourcesDictionary().AddFormXObjectMapping(inImageObject->GetObjectID());

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,0);
		pageContentContext->Do(imageXObjectName);
		pageContentContext->Q();

		delete inImageObject;

		status = inpdfWriter.EndPageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context, for Image form xobject"<<inImageObject->GetObjectID()<<"\n";
			break;
		}

		status = inpdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page, for image form xobject "<<inImageObject->GetObjectID()<<"\n";
			break;
		}
	}while(false);

	return status;
}


EStatusCode TiffSpecialsTest::CreateBlackAndWhiteMaskImage(const TestConfiguration& inTestConfiguration,PDFWriter& inpdfWriter)
{
    string scJimBW = RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jim___ah.tif");

	EStatusCode status = PDFHummus::eSuccess;
	TIFFUsageParameters TIFFParameters;

	do
	{
		PDFFormXObject* imageBW = inpdfWriter.CreateFormXObjectFromTIFFFile(scJimBW);
		if(!imageBW)
		{
			cout<<"failed to create image BW, for file "<<scJimBW<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		TIFFParameters.BWTreatment.AsImageMask = true;
		TIFFParameters.BWTreatment.OneColor = CMYKRGBColor(255,128,0);
		PDFFormXObject* imageBWMask = inpdfWriter.CreateFormXObjectFromTIFFFile(scJimBW,TIFFParameters);
		if(!imageBWMask)
		{
			cout<<"failed to create image mask BW, for file "<<scJimBW<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inpdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
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

		status = inpdfWriter.EndPageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context, for Image BWs test\n";
			break;
		}

		status = inpdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page, for image BWs test\n";
			break;
		}
	}while(false);

	return status;
}

EStatusCode TiffSpecialsTest::CreateBiLevelGrayScales(const TestConfiguration& inTestConfiguration,PDFWriter& inpdfWriter)
{
	EStatusCode status = PDFHummus::eSuccess;
	TIFFUsageParameters TIFFParameters;
    
    string scWJim = RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jim___cg.tif");

	do
	{

		// GrayScale regular
		PDFFormXObject* imageGrayScale = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim);
		if(!imageGrayScale)
		{
			cout<<"failed to create image form XObject from file, grayscale for file "<<scWJim;
			status = PDFHummus::eFailure;
			break;
		}

		// GrayScale Green
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(0,255,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(255,255,255);
		PDFFormXObject* imageGrayScaleGreen = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleGreen)
		{
			cout<<"failed to create image form XObject from file, green n white for file "<<scWJim;
			status = PDFHummus::eFailure;
			break;
		}


		// grayscale cyan magenta
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(255,255,0,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0,0,0,0);
		PDFFormXObject* imageGrayScaleCyanMagenta = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleCyanMagenta)
		{
			cout<<"failed to create image form XObject from file, cyan magenta n white for file "<<scWJim;
			status = PDFHummus::eFailure;
			break;
		}

		// grayscale green vs red
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(0,255,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(255,0,0);
		PDFFormXObject* imageGrayScaleGreenVSRed = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleGreenVSRed)
		{
			cout<<"failed to create image form XObject from file, green n red for file "<<scWJim;
			status = PDFHummus::eFailure;
			break;
		}

		// grayscale cyan vs magenta
		TIFFParameters.GrayscaleTreatment.AsColorMap = true;
		TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(255,0,0,0);
		TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0,255,0,0);
		PDFFormXObject* imageGrayScaleCyanVSMagenta = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim,TIFFParameters);
		if(!imageGrayScaleCyanVSMagenta)
		{
			cout<<"failed to create image form XObject from file, cyan n magenta for file "<<scWJim;
			status = PDFHummus::eFailure;
			break;
		}

		// start page
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inpdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
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

		status = inpdfWriter.EndPageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context, for bilevel grayscale images\n";
			break;
		}

		status = inpdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page, for bilevel grayscale images\n";
			break;
		}
	}while(false);

	return status;
}

ADD_CATEGORIZED_TEST(TiffSpecialsTest,"PDF Images")


#endif