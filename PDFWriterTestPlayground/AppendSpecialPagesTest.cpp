#include "AppendSpecialPagesTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"

#include <iostream>

using namespace std;

AppendSpecialPagesTest::AppendSpecialPagesTest(void)
{
}

AppendSpecialPagesTest::~AppendSpecialPagesTest(void)
{
}

EStatusCode AppendSpecialPagesTest::Run()
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\AppendSpecialPagesTest.PDF",ePDFVersion13,LogConfiguration(true,L"c:\\pdflibtests\\AppendSpecialPagesTestLog.txt"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		EStatusCodeAndObjectIDTypeList result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\AddedItem.pdf",PDFPageRange());
		if(result.first != eSuccess)
		{
			wcout<<"failed to append pages from AddedItem.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\AddedPage.pdf",PDFPageRange());
		if(result.first != eSuccess)
		{
			wcout<<"failed to append pages from AddedPage.pdf\n";
			status = result.first;
			break;
		}


		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\MultipleChange.pdf",PDFPageRange());
		if(result.first != eSuccess)
		{
			wcout<<"failed to append pages from MultipleChange.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\RemovedItem.pdf",PDFPageRange());
		if(result.first != eSuccess)
		{
			wcout<<"failed to append pages from RemovedItem.pdf\n";
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

ADD_CATEGORIZED_TEST(AppendSpecialPagesTest,"PDFEmbedding")
