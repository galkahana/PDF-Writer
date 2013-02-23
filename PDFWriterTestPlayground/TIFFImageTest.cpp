/*
   Source File : TIFFImageTest.cpp


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

#include "TIFFImageTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "BoxingBase.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

TIFFImageTest::TIFFImageTest(void)
{
}

TIFFImageTest::~TIFFImageTest(void)
{
}

EStatusCode TIFFImageTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"BasicTIFFImagesTest.PDF"),ePDFVersion13,LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TiffImageTestLog.txt")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		for(int i=1;i<9 && status != PDFHummus::eFailure;++i)
			status = AddPageForTIFF(pdfWriter,(string(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/CCITT_")) + Int(i).ToString() + ".TIF").c_str());
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/FLAG_T24.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/G4.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/G4S.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/G31D.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/G31DS.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/G32D.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/GMARBLES.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/MARBIBM.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/MARBLES.TIF"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/XING_T24.TIF"));
		if(status != PDFHummus::eSuccess)
			break;

		// tiled image
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/cramps-tile.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/cramps.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		// Looks corrupted by the tool. on mine looks good
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/dscf0013.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		// Creates bad PDF by tool. on mine looks good
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/fax2d.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/g3test.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jello.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jim___ah.tif"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jim___cg.tif"));
		if(status != PDFHummus::eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jim___dg.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/jim___gg.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/oxford.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/quad-lzw.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/quad-tile.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/strike.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/ycbcr-cat.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		for(int i=2;i<9 && status != PDFHummus::eFailure;i=i*2)
			status = AddPageForTIFF(pdfWriter,(string(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/flower-minisblack-")) + Int(i).ToString() + ".tif").c_str());
		if(status !=PDFHummus::eSuccess)
			break;

		for(int i=2;i<9 && status != PDFHummus::eFailure;i=i*2)
			status = AddPageForTIFF(pdfWriter,(string(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/flower-palette-")) + Int(i).ToString() + ".tif").c_str());
		if(status !=PDFHummus::eSuccess)
			break;

		for(int i=2;i<9 && status != PDFHummus::eFailure;i=i*2)
			status = AddPageForTIFF(pdfWriter,(string(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/flower-rgb-contig-")) + Int(i).ToString() + ".tif").c_str());
		if(status !=PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/flower-rgb-planar-8.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/flower-separated-contig-8.tif"));
		if(status != PDFHummus::eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/tiff/flower-separated-planar-8.tif"));
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

EStatusCode TIFFImageTest::AddPageForTIFF(PDFWriter& inpdfWriter, const string& inTiffFilePath)
{
	EStatusCode status = PDFHummus::eSuccess;

	do {
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inpdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page, for file"<<inTiffFilePath.c_str()<<"\n";
		}

		PDFFormXObject* imageFormXObject = inpdfWriter.CreateFormXObjectFromTIFFFile(inTiffFilePath.c_str());
		if(!imageFormXObject)
		{
			cout<<"failed to create image form XObject from file, for file"<<inTiffFilePath.c_str()<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		string imageXObjectName = page->GetResourcesDictionary().AddFormXObjectMapping(imageFormXObject->GetObjectID());

		// continue page drawing, place the image in 0,0 (playing...could avoid CM at all)
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,0);
		pageContentContext->Do(imageXObjectName);
		pageContentContext->Q();

		delete imageFormXObject;

		status = inpdfWriter.EndPageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context, for file"<<inTiffFilePath.c_str()<<"\n";
			break;
		}

		status = inpdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page, for file"<<inTiffFilePath.c_str()<<"\n";
			break;
		}
	}while(false);

	return status;
}

ADD_CATEGORIZED_TEST(TIFFImageTest,"PDF Images")

#endif
