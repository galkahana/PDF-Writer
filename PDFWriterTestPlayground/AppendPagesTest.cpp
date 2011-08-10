/*
   Source File : AppendPagesTest.cpp


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
#include "AppendPagesTest.h"
#include "TestsRunner.h"
#include "HummusPDFWriter.h"

#include <iostream>

using namespace std;

AppendPagesTest::AppendPagesTest(void)
{
}

AppendPagesTest::~AppendPagesTest(void)
{
}

EPDFStatusCode AppendPagesTest::Run()
{
	EPDFStatusCode status;

	do
	{
	 	HummusPDFWriter pdfWriter;

		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\AppendPagesTest.PDF",ePDFVersion13,LogConfiguration(true,L"c:\\pdflibtests\\AppendPagesTestLog.txt"));
		if(status != ePDFSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		EPDFStatusCodeAndObjectIDTypeList result;
		
		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\Original.pdf",PDFPageRange());
		if(result.first != ePDFSuccess)
		{
			wcout<<"failed to append pages from Original.PDF\n";
			status = result.first;
			break;
		}
		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\XObjectContent.PDF",PDFPageRange());
		if(result.first != ePDFSuccess)
		{
			wcout<<"failed to append pages from XObjectContent.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(L"C:\\PDFLibTests\\TestMaterials\\BasicTIFFImagesTest.PDF",PDFPageRange());
		if(result.first != ePDFSuccess)
		{
			wcout<<"failed to append pages from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}
		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}

	}while(false);

	return status;
}


ADD_CATEGORIZED_TEST(AppendPagesTest,"PDFEmbedding")