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


int CIDSetWritingTrueType(int argc, char* argv[])
{
	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"CIDSetWritingTrueType.pdf"),ePDFVersion14);
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
                                                                            "fonts/yugothib.ttf")),
																			14,
																			AbstractContentContext::eGray,
																			0);

		cxt->WriteText(75,600,"hello \xe6\xb8\xb8\xe7\xaf\x89\xe8\xa6\x8b\xe5\x87\xba\xe3\x81\x97\xe6\x98\x8e\xe6\x9c\x9d\xe4\xbd\x93",textOptions);

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
