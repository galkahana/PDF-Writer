/*
   Source File : CIDSetWritingCFF.cpp


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


int TrueTypeAnsiWriteBug(int argc, char* argv[])
{
	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"TrueTypeAnsiWriteBug.pdf"),ePDFVersion14);
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
                                                                            "fonts/HelveticaNeue.ttc")),
																			14,
																			AbstractContentContext::eGray,
																			0);

		cxt->WriteText(75,600,"Helvetica Neue",textOptions);

 		GlyphUnicodeMappingList guml;
		// Helve, but with direct glyphs writing
 		guml.push_back(GlyphUnicodeMapping(47, 72));
 		guml.push_back(GlyphUnicodeMapping(76, 101));
 		guml.push_back(GlyphUnicodeMapping(83, 108));
 		guml.push_back(GlyphUnicodeMapping(93, 118));
 		guml.push_back(GlyphUnicodeMapping(76, 101));
 		cxt->BT();
 		cxt->k(0,0,0,1);
 		cxt->Tm(1,0,0,1,10,600);
 		cxt->Tf(textOptions.font, 14);
 		cxt->Tj(guml);
 		cxt->ET();		

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
