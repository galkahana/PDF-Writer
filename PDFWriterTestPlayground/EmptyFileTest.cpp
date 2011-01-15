#include "EmptyFileTest.h"
#include "PDFWriter.h"


#include <iostream>

using namespace std;


EmptyFileTest::EmptyFileTest(void)
{
}

EmptyFileTest::~EmptyFileTest(void)
{
}

EStatusCode EmptyFileTest::Run()
{
	PDFWriter pdfWriter;
	LogConfiguration logConfiguration(false,L"C:\\PDFLibTests\\EmptyFileLog.txt");
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\test.txt",ePDFVersion13,logConfiguration);
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
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

ADD_CETEGORIZED_TEST(EmptyFileTest,"PDF")
