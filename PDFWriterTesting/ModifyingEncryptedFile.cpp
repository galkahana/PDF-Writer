/*
 Source File : ModifyingEncryptedFile.cpp
 
 
 Copyright 2012 Gal Kahana PDFWriter
 
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
#include "PDFModifiedPage.h"
#include "AbstractContentContext.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace PDFHummus;

static EStatusCode RunTest(char* argv[],PDFWriter& pdfWriter)
{

	EStatusCode status = eSuccess;

	do
	{

		/*
		modify a password protected file. retain the same protection, just add some content
		*/

		// open file for modification
		if (status != eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		// modify first page to include text
		{
			PDFModifiedPage modifiedPage(&pdfWriter, 0);

			AbstractContentContext* contentContext = modifiedPage.StartContentContext();
			AbstractContentContext::TextOptions opt(
				pdfWriter.GetFontForFile(BuildRelativeInputPath(argv,
					"fonts/arial.ttf")),
				14,
				AbstractContentContext::eGray,
				0
				);

			contentContext->WriteText(10, 805, "new text on encrypted page!", opt);

			modifiedPage.EndContentContext();
			modifiedPage.WritePage();
		}

		// add new page with an image
		{
			PDFPage* page = new PDFPage();
			page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

			PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
			if (NULL == contentContext)
			{
				status = PDFHummus::eFailure;
				cout << "failed to create content context for page\n";
				break;
			}

			contentContext->DrawImage(10, 300, BuildRelativeInputPath(argv, "images/soundcloud_logo.jpg"));

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

		}

		status = pdfWriter.EndPDF();
		if (status != eSuccess)
		{
			cout << "failed in end PDF\n";
			break;
		}
	} while (false);

	return status;
}

int ModifyingEncryptedFile(int argc, char* argv[])
{
	EStatusCode status = eSuccess;

	do {
		{
			PDFWriter pdfWriter;
			status = pdfWriter.ModifyPDF(
				BuildRelativeInputPath(argv,string("PDFWithPassword.pdf")),
				ePDFVersion13,
				BuildRelativeOutputPath(argv,string("PDFWithPasswordEcnryptedModified.pdf")),
				LogConfiguration(true, true, BuildRelativeOutputPath(argv,string("PDFWithPasswordEcnryptedModified.log"))),
				PDFCreationSettings(true, true, EncryptionOptions("user", 0, "")));

			status = RunTest(argv, pdfWriter);
			if (status != eSuccess) {
				break;
			}
		}

		{
			PDFWriter pdfWriter;
			status = pdfWriter.ModifyPDF(
				BuildRelativeInputPath(argv,string("china.pdf")),
				ePDFVersion13,
				BuildRelativeOutputPath(argv,string("chinaEcnryptedModified.pdf")),
				LogConfiguration(true, true, BuildRelativeOutputPath(argv,string("chinaEcnryptedModified.log"))),
				PDFCreationSettings(true, true));

			status = RunTest(argv, pdfWriter);
			if (status != eSuccess) {
				break;
			}
		}
	} while (false);

	return status == eSuccess ? 0:1;
}


