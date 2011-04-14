#include "AppendPagesTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"

#include <iostream>

using namespace std;

AppendPagesTest::AppendPagesTest(void)
{
}

AppendPagesTest::~AppendPagesTest(void)
{
}

EStatusCode AppendPagesTest::Run()
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\AppendPagesTest.PDF",ePDFVersion13,LogConfiguration(true,L"c:\\PDFLibTests\\zvika.txt"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		// append pages from XObjectContent.pdf
		EStatusCodeAndObjectIDTypeList result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\XObjectContent.PDF",PDFPageRange());
		if(result.first != eSuccess)
		{
			wcout<<"failed to create append pages from XObjectContent.pdf\n";
			status = result.first;
			break;
		}


		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\BasicTIFFImagesTest.PDF",PDFPageRange());
		if(result.first != eSuccess)
		{
			wcout<<"failed to create append pages from BasicTIFFImagesTest.PDF\n";
			status = result.first;
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


ADD_CETEGORIZED_TEST(AppendPagesTest,"PDFEmbedding")