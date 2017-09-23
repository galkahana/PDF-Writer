/*
   Source File : EmptyPagesPDF.cpp


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
#include "PDFWithPassword.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

PDFWithPassword::PDFWithPassword(void)
{
}

PDFWithPassword::~PDFWithPassword(void)
{
}

EStatusCode RunTest(const TestConfiguration& inTestConfiguration, bool inUseAES)
{
	PDFWriter pdfWriter;
	LogConfiguration logConfiguration(true, true, RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, inUseAES ? "PDFWithPasswordAESLog.txt":"PDFWithPasswordRC4Log.txt"));
	EStatusCode status;
	PDFPage* page = NULL;

	do
	{
		status = pdfWriter.StartPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, inUseAES ?  "PDFWithPasswordAES.pdf" : "PDFWithPasswordRC4.pdf"),
			inUseAES ? ePDFVersion16 : ePDFVersion14,
			logConfiguration,
			PDFCreationSettings(true, true, EncryptionOptions("user", 4, "owner"))); // 4 should translate to -3900 in actual PDF
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		// create simple page
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* cxt = pdfWriter.StartPageContentContext(page);

		// draw an image
		cxt->DrawImage(10, 100, RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/images/soundcloud_logo.jpg"));

		// place some text
		AbstractContentContext::TextOptions textOptions(pdfWriter.GetFontForFile(
			RelativeURLToLocalPath(
				inTestConfiguration.mSampleFileBase,
				"TestMaterials/fonts/arial.ttf")),
			14,
			AbstractContentContext::eGray,
			0);
		cxt->WriteText(10, 50, "Hello", textOptions);

		status = pdfWriter.EndPageContentContext(cxt);

		if (status != PDFHummus::eSuccess) {
			cout << "failed end context\n";
			break;
		}


		status = pdfWriter.WritePageAndRelease(page);
		page = NULL;

		if (status != PDFHummus::eSuccess) {
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

	// failure cleanups
	delete page;

	return status;
}

EStatusCode PDFWithPassword::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;

	do
	{
		status = RunTest(inTestConfiguration, false);
		if (status != eSuccess) {
			cout << "failed to run RC4 test\n";
			break;
		}

		status = RunTest(inTestConfiguration, true);
		if (status != eSuccess) {
			cout << "failed to run AES test\n";
			break;
		}

	}while(false);

	return status;
}


ADD_CATEGORIZED_TEST(PDFWithPassword,"Xcryption")
