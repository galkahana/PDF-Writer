/*
   Source File : SimpleTextUsage.cpp


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
#include "UnicodeTextUsage.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "TestsRunner.h"

#include <iostream>

using namespace PDFHummus;

UnicodeTextUsage::UnicodeTextUsage(void)
{
}

UnicodeTextUsage::~UnicodeTextUsage(void)
{
}



EStatusCode UnicodeTextUsage::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"UnicodeTextUsage.pdf"),
			ePDFVersion13,
			LogConfiguration(true, true, RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "UnicodeTextUsage.log")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if (NULL == contentContext)
		{
			status = PDFHummus::eFailure;
			cout << "failed to create content context for page\n";
			break;
		}

		PDFUsedFont* font = pdfWriter.GetFontForFile(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/fonts/arial.ttf"));
		if (!font)
		{
			status = PDFHummus::eFailure;
			cout << "Failed to create font object for arial.ttf\n";
			break;
		}


		// Draw some text
		contentContext->BT();
		contentContext->k(0, 0, 0, 1);

		contentContext->Tf(font, 1);

		contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);

		EStatusCode encodingStatus = contentContext->Tj("hello \xD7\x92");
		if (encodingStatus != PDFHummus::eSuccess)
			cout << "Could not find some of the glyphs for this font";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		status = pdfWriter.EndPageContentContext(contentContext);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to write page\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed in end PDF\n";
			break;
		}
	} while (false);
	return status;
}



ADD_CATEGORIZED_TEST(UnicodeTextUsage,"PDF")
