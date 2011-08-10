/*
   Source File : EmptyPagesPDF.cpp


   Copyright 2011 Gal Kahana HummusPDFWriter

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
#include "EmptyPagesPDF.h"
#include "HummusPDFWriter.h"
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

EPDFStatusCode EmptyPagesPDF::Run()
{
	HummusPDFWriter pdfWriter;
	LogConfiguration logConfiguration(false,L"C:\\PDFLibTests\\EmptyPagesLog.txt");
	EPDFStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\EmptyPages.PDF",ePDFVersion13,logConfiguration);
		if(status != ePDFSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	


		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		for(int i=0;i<4 && ePDFSuccess == status;++i)
		{
			status = pdfWriter.WritePage(page);
			if(status != ePDFSuccess)
				wcout<<"failed to write page "<<i<<"\n";
		}
		delete page;		

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
}


ADD_CATEGORIZED_TEST(EmptyPagesPDF,"PDF")
