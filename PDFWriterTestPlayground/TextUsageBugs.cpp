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

		status = RunKoreanFontTest(inTestConfiguration);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Error in RunKoreanFontTest\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunCNRSTest(inTestConfiguration);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Error in RunCNRSTest\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunCNRS2Test(inTestConfiguration);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Error in RunCNRS2Test\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = RunCNRS2SampleTest(inTestConfiguration);
		if (status != PDFHummus::eSuccess)
		{
			cout << "Error in RunCNRS2SampleTest\n";
			status = PDFHummus::eFailure;
			break;
		}

	} while(false);
	return status;
}

EStatusCode TextUsageBugs::RunKoreanFontTest(const TestConfiguration& inTestConfiguration) {
	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "KoreanFontTest.pdf"),
			ePDFVersion13,
			LogConfiguration(true, true,
				RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "KoreanFontTest.log")));
		if (status != eSuccess)
		{
			cout << "Failed to start file\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* cxt = pdfWriter.StartPageContentContext(page);

		AbstractContentContext::TextOptions textOptions(pdfWriter.GetFontForFile(
			RelativeURLToLocalPath(
				inTestConfiguration.mSampleFileBase,
				"TestMaterials/fonts/NotoSerifCJKkr-Regular.otf")),
			14,
			AbstractContentContext::eGray,
			0);

		cxt->WriteText(10, 200, "hello", textOptions);
		cxt->WriteText(10, 400, "\x48\x65\x6C\x6C\x6F\x20\x57\x6F\x72\x6C\x64\x20\xED\x95\x9C\xEA\xB8\x80\x20\x61\x62\x63\x21", textOptions);

		status = pdfWriter.EndPageContentContext(cxt);
		if (status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout << "Failed to end content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if (status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout << "Failed to write page\n";
			break;
		}


		status = pdfWriter.EndPDF();
		if (status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout << "Failed to end pdf\n";
			break;
		}

	} while (false);


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

EStatusCode TextUsageBugs::RunCNRS2SampleTest(const TestConfiguration& inTestConfiguration) {
	PDFWriter pdfWriter;
	EStatusCode status;

	do {
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TextUsageBugsCNRSSample.pdf"), ePDFVersion14);
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
		contentContext->Tf(font, 8);

		contentContext->Tm(1, 0, 0, 1, 10, 680);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3014, 8600));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2963, 10229));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3205, 10232));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3049, 10235));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3068, 8617));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2964, 10230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3206, 10233));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3050, 10236));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2994, 10231));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3224, 10234));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2964, 10230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2964, 10230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3206, 10233));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3206, 10233));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3050, 10236));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3050, 10236));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3795, 172));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3847, 8709));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3703, 8728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4066, 9633));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(115, 8226));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4067, 9632));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3805, 8868));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3802, 8869));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3808, 8866));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3810, 8867));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3815, 8873));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3816, 8874));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3814, 8872));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4109, 9837));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4107, 9839));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4108, 9838));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4102, 9830));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4100, 9829));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4098, 9824));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4104, 9827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(120, 8230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3700, 8943));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(120, 8230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3699, 8942));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3702, 8945));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 660);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3701, 8944));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4149, 201330741));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4150, 201330742));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4151, 201330743));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4153, 201330745));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4154, 201330746));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4216, 201330808));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4215, 201330807));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4217, 201330809));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3868, 201330460));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3873, 201330465));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3864, 201330456));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3829, 201330421));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3827, 201330419));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3861, 201330453));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3854, 201330446));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3852, 201330444));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3854, 201330446));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3852, 201330444));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3835, 201330427));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3833, 201330425));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(127, 175));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(135, 711));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(128, 728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(128, 728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(36, 67));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(36, 67));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(36, 67));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3651, 10216));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }

		contentContext->Tm(1, 0, 0, 1, 10, 640);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3652, 10217));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(18, 49));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(18, 49));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1706, 931));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 620);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3871, 8855));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }

		contentContext->Tm(1, 0, 0, 1, 10, 600);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 580);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 560);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 540);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 520);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 500);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }


		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 480);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }

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


EStatusCode TextUsageBugs::RunCNRS2Test(const TestConfiguration& inTestConfiguration) {
	PDFWriter pdfWriter;
	EStatusCode status;

	do {
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TextUsageBugsCNRSFinal.pdf"), ePDFVersion14);
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
		contentContext->Tf(font, 8);

		contentContext->Tm(1, 0, 0, 1, 10, 780);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1792, 120572));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1804, 120573));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1876, 120574));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1828, 120575));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1840, 120576));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1852, 120598));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2152, 120577));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1864, 120578));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2104, 120579));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2116, 120599));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1888, 120580));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1900, 120581));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1912, 120600));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1924, 120582));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1936, 120583));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1948, 120584));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2140, 120585));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1972, 120586));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2008, 120587));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2020, 120603));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2044, 120588));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2056, 120602));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2068, 120590));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2080, 120589));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2092, 120591));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2128, 120592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1984, 120593));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1996, 120601));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2032, 120595));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1816, 120594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1960, 120596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1490, 913));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1502, 914));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1562, 915));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1526, 916));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1538, 917));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1766, 918));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1550, 919));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1730, 920));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1574, 921));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1586, 922));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1598, 923));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1610, 924));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1622, 925));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1754, 926));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1646, 927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1670, 928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1694, 929));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1706, 931));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1718, 932));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1742, 933));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1658, 934));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1682, 936));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1514, 935));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1634, 937));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(69, 100));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2162, 8706));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(74, 105));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1874, 947));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1922, 955));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2006, 960));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1562, 915));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(624, 8463));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(144, 305));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(436, 8467));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1778, 8711));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1526, 916));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4056, 8734));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2753, 8476));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2744, 8465));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3844, 8705));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3843, 8704));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3845, 8707));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3845, 8707));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3845, 8707));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3845, 8707));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3845, 8707));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3845, 8707));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4059, 8472));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2189, 8501));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2190, 8502));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2191, 8503));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2192, 8504));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1958, 969));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1538, 917));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2176, 8487));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3866, 8853));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3869, 8854));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3871, 8855));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3870, 8856));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3862, 8857));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3865, 8858));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3879, 8862));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3880, 8863));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3881, 8864));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }

		contentContext->Tm(1, 0, 0, 1, 10, 760);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3878, 8865));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3723, 8727));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3687, 177));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3688, 8723));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3706, 215));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3691, 247));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3723, 8727));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3724, 8902));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3703, 8728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(115, 8226));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3824, 8745));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3825, 8746));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3859, 8846));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3693, 8901));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3849, 8851));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3850, 8852));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3831, 8744));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3830, 8743));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3840, 8910));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3839, 8911));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3838, 8891));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3902, 8965));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3719, 8905));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3720, 8906));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3721, 8907));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3722, 8908));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4002, 8764));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4007, 8771));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4013, 8776));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4010, 8773));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4004, 8769));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4009, 8772));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4014, 8777));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4012, 8775));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3895, 8781));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3886, 8801));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4010, 8773));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4002, 8764));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4002, 8764));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3896, 8813));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3890, 8802));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4012, 8775));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4002, 8764));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3683, 8722));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4002, 8764));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3898, 8783));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3897, 8782));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3913, 8791));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3912, 8790));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4003, 8765));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4008, 8909));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4002, 8764));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4013, 8776));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3885, 8800));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4057, 8733));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4015, 8778));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3915, 8796));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(29, 60));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3939, 10877));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3923, 8804));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3929, 8806));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(31, 62));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3940, 10878));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3924, 8805));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 740);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3930, 8807));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3947, 8810));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3923, 8804));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3923, 8804));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3949, 8920));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3923, 8804));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3923, 8804));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3923, 8804));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3948, 8811));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3924, 8805));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3924, 8805));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3950, 8921));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3924, 8805));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3924, 8805));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3924, 8805));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3953, 8828));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4028, 8830));
		contentContext->Tj(glyph); }



		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3954, 8829));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4029, 8831));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }

		contentContext->Tm(1, 0, 0, 1, 10, 720);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3975, 8834));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3979, 8838));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3975, 8834));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3987, 8712));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3976, 8835));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3980, 8839));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3976, 8835));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3988, 8715));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3991, 8847));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3993, 8849));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3999, 8912));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3975, 8834));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3992, 8848));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3994, 8850));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4000, 8913));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3976, 8835));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3963, 8882));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3965, 8884));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3964, 8883));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3966, 8885));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4018, 8818));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4024, 10885));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4028, 8830));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4013, 8776));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4019, 8819));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4025, 10886));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4019, 8819));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4025, 10886));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3989, 8713));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3990, 8716));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3919, 8814));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3939, 10877));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3927, 8816));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3933, 10887));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3920, 8815));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3940, 10878));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3928, 8817));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3934, 10888));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3931, 8808));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3931, 8808));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4022, 8934));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4026, 10889));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3932, 8809));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3932, 8809));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4023, 8935));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4027, 10890));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3983, 8842));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3975, 8834));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3983, 8842));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 700);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3975, 8834));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3984, 8843));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3976, 8835));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3984, 8843));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3976, 8835));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3955, 8832));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3959, 8928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3961, 10927));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3956, 8833));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3960, 8929));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3962, 10928));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(30, 61));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4030, 8936));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3951, 8826));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4013, 8776));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3991, 8847));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3995, 8930));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4031, 8937));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3952, 8827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4013, 8776));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(16, 47));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3992, 8848));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3996, 8931));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3103, 8636));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3113, 8637));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3114, 8641));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3104, 8640));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3193, 8651));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3194, 8652));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3163, 8647));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3037, 8610));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3068, 8617));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3017, 8606));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3164, 8649));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3038, 8611));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3018, 8608));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3144, 8646));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3143, 8644));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3078, 8619));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3077, 8620));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3283, 8621));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2971, 8593));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2998, 8597));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3213, 8657));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3173, 8648));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2972, 8595));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3228, 8661));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3214, 8659));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3174, 8650));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3133, 8639));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3123, 8638));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3009, 8598));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3013, 8599));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3134, 8643));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3124, 8642));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3010, 8601));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 680);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3014, 8600));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2963, 10229));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3205, 10232));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3049, 10235));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3068, 8617));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2964, 10230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3206, 10233));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3050, 10236));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2994, 10231));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3224, 10234));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2964, 10230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2964, 10230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3206, 10233));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3206, 10233));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3050, 10236));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3050, 10236));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3067, 8618));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3795, 172));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3847, 8709));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3703, 8728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4066, 9633));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(115, 8226));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4067, 9632));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3805, 8868));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3802, 8869));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3808, 8866));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3810, 8867));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3815, 8873));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3816, 8874));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3814, 8872));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4109, 9837));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4107, 9839));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4108, 9838));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4102, 9830));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4100, 9829));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4098, 9824));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4104, 9827));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(120, 8230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3700, 8943));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(120, 8230));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3699, 8942));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3702, 8945));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 660);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3701, 8944));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4149, 201330741));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4150, 201330742));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4151, 201330743));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4153, 201330745));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4154, 201330746));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4216, 201330808));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4215, 201330807));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(4217, 201330809));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3868, 201330460));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3873, 201330465));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3864, 201330456));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3829, 201330421));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3827, 201330419));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3861, 201330453));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3854, 201330446));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3852, 201330444));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3854, 201330446));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3852, 201330444));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3835, 201330427));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3833, 201330425));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(127, 175));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(135, 711));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(128, 728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(128, 728));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(36, 67));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(36, 67));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(34, 65));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(35, 66));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(36, 67));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(37, 68));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3651, 10216));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }

		contentContext->Tm(1, 0, 0, 1, 10, 640);

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(93, 124));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3652, 10217));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(18, 49));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(70, 101));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(18, 49));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(9, 40));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(10, 41));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(1706, 931));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 620);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3871, 8855));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(15, 46));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }

		contentContext->Tm(1, 0, 0, 1, 10, 600);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 580);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2962, 8594));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 560);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2961, 8592));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(2993, 8596));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 540);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3048, 8614));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 520);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3203, 8656));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 500);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }

		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3204, 8658));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }


		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		contentContext->Tm(1, 0, 0, 1, 10, 480);
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(3223, 8660));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }
		{ GlyphUnicodeMappingList glyph;
		glyph.push_back(GlyphUnicodeMapping(12, 43));
		contentContext->Tj(glyph); }

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
