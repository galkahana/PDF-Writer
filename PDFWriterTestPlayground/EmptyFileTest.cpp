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
		status = pdfWriter.InitializePDFWriter(L"C:\\PDFLibTests\\test.txt",logConfiguration);
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
