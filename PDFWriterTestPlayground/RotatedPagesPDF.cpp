/*
   Source File : RotatedPagesPDF.cpp


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
#include "RotatedPagesPDF.h"
#include "PageContentContext.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PDFPageInput.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

RotatedPagesPDF::RotatedPagesPDF(void)
{
}

RotatedPagesPDF::~RotatedPagesPDF(void)
{
}

EStatusCode RotatedPagesPDF::Run(const TestConfiguration& inTestConfiguration)
{
	LogConfiguration logConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"RotatedPagesLog.txt"));
	EStatusCode status; 

	do
	{
		// PDF Page rotation writing

		PDFWriter pdfWriter;
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"RotatedPages.PDF"),ePDFVersion13,logConfiguration);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		AbstractContentContext::TextOptions textOptions(pdfWriter.GetFontForFile(
																			RelativeURLToLocalPath(
                                                                            inTestConfiguration.mSampleFileBase,
                                                                            "TestMaterials/fonts/arial.ttf")),
																			14,
																			AbstractContentContext::eGray,
																			0);

		for(int i=0;i<4 && PDFHummus::eSuccess == status;++i)
		{
			PDFPage page;
			page.SetMediaBox(PDFRectangle(0,0,595,842));
			page.SetRotate(i*90);
		
			std::ostringstream s;
			s << "Page rotated by " << i*90 << " degrees.";

			PageContentContext* cxt = pdfWriter.StartPageContentContext(&page);
			cxt->WriteText(75,805,s.str(),textOptions);
			status = pdfWriter.EndPageContentContext(cxt);
			if(status != eSuccess)
			{
				status = PDFHummus::eFailure;
				cout<<"Failed to end content context\n";
				break;
			}

			status = pdfWriter.WritePage(&page);
			if(status != PDFHummus::eSuccess)
				cout<<"failed to write page "<<i<<"\n";
		}

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}


		// PDF Page rotation parsing

		InputFile pdfFile;
		PDFParser pdfParser;

		status = pdfFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"RotatedPages.PDF"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to open file for reading. should be in TestMaterials/XObjectContent.PDF\n";
			break;
		}

		status = pdfParser.StartPDFParsing(pdfFile.GetInputStream());
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to parse input file";
			break;
		}

		if(pdfParser.GetPagesCount() != 4)
		{
			cout<<"expecting 4 pages, got "<<pdfParser.GetPagesCount()<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		for(int i=0;i<4 && PDFHummus::eSuccess == status;++i)
		{
			RefCountPtr<PDFDictionary> page = pdfParser.ParsePage(i);
			if (!page)
			{
				cout << i << ". page parsing failed\n";
				status = PDFHummus::eFailure;
				break;
			}

			PDFPageInput input( &pdfParser, page );
			if ( input.GetRotate() != i*90 )
			{
				cout<< i << ". page has invalid rotation\n";
				status = PDFHummus::eFailure;
				break;
			}
		}
		
	}while(false);
	return status;
}


ADD_CATEGORIZED_TEST(RotatedPagesPDF,"PDF")
