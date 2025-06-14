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
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "BoxingBase.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

static EStatusCode RunTest(char* argv[], EPDFVersion pdfVersion)
{
	PDFWriter pdfWriter;
	string pdfFileName = string("PDFWithPassword") + Int(pdfVersion).ToString();
	LogConfiguration logConfiguration(true, true, BuildRelativeOutputPath(argv, pdfFileName + ".txt"));
	EStatusCode status;
	PDFPage* page = NULL;

	do
	{
		status = pdfWriter.StartPDF(
			BuildRelativeOutputPath(argv,pdfFileName + ".pdf"),
			pdfVersion,
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
		cxt->DrawImage(10, 100, BuildRelativeInputPath(argv, "images/soundcloud_logo.jpg"));

		// place some text
		AbstractContentContext::TextOptions textOptions(pdfWriter.GetFontForFile(
			BuildRelativeInputPath(
				argv,
				"fonts/arial.ttf")),
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

int PDFWithPassword(int argc, char* argv[])
{
	EStatusCode status;

	do
	{
		status = RunTest(argv, ePDFVersion14);
		if (status != eSuccess) {
			cout << "failed to run RC4 test\n";
			break;
		}

		status = RunTest(argv, ePDFVersion16);
		if (status != eSuccess) {
			cout << "failed to run AES test\n";
			break;
		}

#ifndef PDFHUMMUS_NO_OPENSSL
		status = RunTest(argv, ePDFVersion20);
		if (status != eSuccess) {
			cout << "failed to run PDF2.0 test\n";
			break;
		}		
#endif
	}while(false);

	return status == eSuccess ? 0:1;
}

