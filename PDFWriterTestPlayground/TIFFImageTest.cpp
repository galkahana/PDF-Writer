#include "TIFFImageTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "BoxingBase.h"

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
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\BasicTIFFImagesTest.PDF",ePDFVersion13,LogConfiguration(true,L"C:\\PDFLibTests\\TiffImageTestLog.txt"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		for(int i=1;i<9 && status != eFailure;++i)
			status = AddPageForTIFF(pdfWriter,(wstring(L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\CCITT_") + Int(i).ToWString() + L".TIF").c_str());
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\FLAG_T24.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\G4.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\G4S.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\G31D.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\G31DS.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\G32D.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\GMARBLES.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\MARBIBM.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\MARBLES.TIF");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\tiffs1\\XING_T24.TIF");
		if(status != eSuccess)
			break;

		// tiled image
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\cramps-tile.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\cramps.tif");
		if(status != eSuccess)
			break;

		// Looks corrupted by the tool. on mine looks good
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\dscf0013.tif");
		if(status != eSuccess)
			break;

		// Creates bad PDF by tool. on mine looks good
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\fax2d.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\g3test.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jello.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___ah.tif");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___cg.tif");
		if(status != eSuccess)
			break;
		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___dg.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\jim___gg.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\oxford.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\quad-lzw.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\quad-tile.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\strike.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\ycbcr-cat.tif");
		if(status != eSuccess)
			break;

		for(int i=2;i<9 && status != eFailure;i=i*2)
			status = AddPageForTIFF(pdfWriter,(wstring(L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\depth\\flower-minisblack-") + Int(i).ToWString() + L".tif").c_str());
		if(status !=eSuccess)
			break;

		for(int i=2;i<9 && status != eFailure;i=i*2)
			status = AddPageForTIFF(pdfWriter,(wstring(L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\depth\\flower-palette-") + Int(i).ToWString() + L".tif").c_str());
		if(status !=eSuccess)
			break;

		for(int i=2;i<9 && status != eFailure;i=i*2)
			status = AddPageForTIFF(pdfWriter,(wstring(L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\depth\\flower-rgb-contig-") + Int(i).ToWString() + L".tif").c_str());
		if(status !=eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\depth\\flower-rgb-planar-8.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\depth\\flower-separated-contig-8.tif");
		if(status != eSuccess)
			break;

		status = AddPageForTIFF(pdfWriter,L"C:\\PDFLibTests\\TestMaterials\\pics-3.7.2\\depth\\flower-separated-planar-8.tif");
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

EStatusCode TIFFImageTest::AddPageForTIFF(PDFWriter& inPDFWriter, const wchar_t* inTiffFilePath)
{
	EStatusCode status = eSuccess;

	do {
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContentContext = inPDFWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page, for file"<<inTiffFilePath<<"\n";
		}

		PDFFormXObject* imageFormXObject = inPDFWriter.CreateFormXObjectFromTIFFFile(inTiffFilePath);
		if(!imageFormXObject)
		{
			wcout<<"failed to create image form XObject from file, for file"<<inTiffFilePath<<"\n";
			status = eFailure;
			break;
		}

		string imageXObjectName = page->GetResourcesDictionary().AddFormXObjectMapping(imageFormXObject->GetObjectID());

		// continue page drawing, place the image in 0,0 (playing...could avoid CM at all)
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,0);
		pageContentContext->Do(imageXObjectName);
		pageContentContext->Q();

		delete imageFormXObject;

		status = inPDFWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context, for file"<<inTiffFilePath<<"\n";
			break;
		}

		status = inPDFWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page, for file"<<inTiffFilePath<<"\n";
			break;
		}
	}while(false);

	return status;
}

ADD_CETEGORIZED_TEST(TIFFImageTest,"PDF Images")
