/*
   Source File : CIDSetWritingTrueType.cpp


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
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFUsedFont.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace PDFHummus;


int CIDSetWritingTrueType2(int argc, char* argv[])
{
	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"CIDSetWritingTrueType2.pdf"),ePDFVersion14);
		if(status != eSuccess)
		{
			cout<<"Failed to start file\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
		
		PageContentContext* cxt = pdfWriter.StartPageContentContext(page);

		AbstractContentContext::TextOptions textOptions(pdfWriter.GetFontForFile(
																			BuildRelativeInputPath(
                                                                            argv,
                                                                            "fonts/segoeui.ttf")),
																			14,
																			AbstractContentContext::eGray,
																			0);

		cxt->WriteText(75,600,"\x53\x65\x67\x6f\x65\x20\x55\x49\x3a\x20\xd9\x84\xd9\x84\xd9\x85\xd8\xb5\xd9\x85\xd9\x85\xd9\x8a\xd9\x86\x20\xd9\x86\xd8\xb5",textOptions);

		status = pdfWriter.EndPageContentContext(cxt);
		if(status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to end content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to write page\n";
			break;
		}


		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to end pdf\n";
			break;
		}

	}while(false);


	return status == eSuccess ? 0:1;
}
