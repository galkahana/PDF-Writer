/*
   Source File : TextUsageBugs.cpp


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
#include "TextUsageBugs.h"
#include "TestsRunner.h"
#include "Trace.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"

#include <iostream>

using namespace PDFHummus;

TextUsageBugs::TextUsageBugs(void)
{
}

TextUsageBugs::~TextUsageBugs(void)
{
}



EStatusCode TextUsageBugs::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;

	// running two versions of the tests, one with font embedding, one without

	do
	{
		status = RunCNRSTest(inTestConfiguration);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Error in RunCNRSTest\n";
			status = PDFHummus::eFailure;
			break;
		}

	}while(false);

	return status;
}

EStatusCode TextUsageBugs::RunCNRSTest(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do {
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TextUsageBugsCNRS.pdf"), ePDFVersion14, LogConfiguration::DefaultLogConfiguration(),PDFCreationSettings(true,true));
		if (status != PDFHummus::eSuccess) {
			cout << "failed to start PDF\n";
			break;
		}
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if (NULL == contentContext) {
			status = PDFHummus::eFailure;
			cout << "failed to create content context for page\n";
			break;
		}
		PDFUsedFont* font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/fonts/texgyrepagella-math.otf"));
		if (!font) {
			status = PDFHummus::eFailure;
			cout << "Failed to create font object\n";
			break;
		}

		contentContext->BT();
		contentContext->k(0, 0, 0, 1);
		contentContext->Tf(font, 10);

		contentContext->Tm(1, 0, 0, 1, 78, 660);
		GlyphUnicodeMappingList glyphs1;
		for (int i = 65; i < 150; i++)
			glyphs1.push_back(GlyphUnicodeMapping(i, i));
		contentContext->Tj(glyphs1);

		contentContext->Tm(1, 0, 0, 1, 78, 640);
		GlyphUnicodeMappingList glyphs2;
		for (int i = 150; i < 210; i++)
			glyphs2.push_back(GlyphUnicodeMapping(i, i));
		contentContext->Tj(glyphs2);

		contentContext->Tm(1, 0, 0, 1, 78, 620);
		GlyphUnicodeMappingList glyphs3;
		for (int i = 210; i < 280; i++)
			glyphs3.push_back(GlyphUnicodeMapping(i, i));
		contentContext->Tj(glyphs3);

		contentContext->Tm(1, 0, 0, 1, 78, 600);
		GlyphUnicodeMappingList glyphs4;
		for (int i = 2900; i < 2950; i++)
			glyphs4.push_back(GlyphUnicodeMapping(i, i));
		contentContext->Tj(glyphs4);

		contentContext->ET();

		status = pdfWriter.EndPageContentContext(contentContext);
		if (status != PDFHummus::eSuccess) {
			cout << "failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if (status != PDFHummus::eSuccess) {
			cout << "failed to write page\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if (status != PDFHummus::eSuccess) {
			cout << "failed in end PDF\n";
			break;
		}
	} while (false);
	return status;
}


ADD_CATEGORIZED_TEST(TextUsageBugs,"PDF")
