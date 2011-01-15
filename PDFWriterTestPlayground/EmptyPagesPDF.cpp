#include "EmptyPagesPDF.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;

EmptyPagesPDF::EmptyPagesPDF(void)
{
}

EmptyPagesPDF::~EmptyPagesPDF(void)
{
}

EStatusCode EmptyPagesPDF::Run()
{
	PDFWriter pdfWriter;
	LogConfiguration logConfiguration(false,L"C:\\PDFLibTests\\EmptyPagesLog.txt");
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\EmptyPages.PDF",ePDFVersion13,logConfiguration);
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	


		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		for(int i=0;i<4 && eSuccess == status;++i)
		{
			status = pdfWriter.WritePage(page);
			if(status != eSuccess)
				wcout<<"failed to write page "<<i<<"\n";
		}
		delete page;		

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
}


ADD_CETEGORIZED_TEST(EmptyPagesPDF,"PDF")