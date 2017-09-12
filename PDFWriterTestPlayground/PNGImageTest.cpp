/*
   Source File : JPGImageTest.cpp


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

#ifndef PDFHUMMUS_NO_PNG

#include "PNGImageTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;



PNGImageTest::PNGImageTest(void)
{
}

PNGImageTest::~PNGImageTest(void)
{
}

EStatusCode RunImageTest(const TestConfiguration& inTestConfiguration, const string& inImageName) {
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PNGTest_" + inImageName + ".pdf"), ePDFVersion14, LogConfiguration(true, true,
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PNGTest_" + inImageName + ".log")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if (NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout << "failed to create content context for page\n";
		}

		// place a large red rectangle all over the page
		AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill,
			AbstractContentContext::eRGB,
			0xFF0000);
		pageContentContext->DrawRectangle(0, 0, 595, 842, pathFillOptions);

		// place the image on top...hopefully we can see soem transparency
		AbstractContentContext::ImageOptions imageOptions;
		imageOptions.transformationMethod = AbstractContentContext::eMatrix;
		imageOptions.matrix[0] = imageOptions.matrix[3] = 0.5;
		pageContentContext->DrawImage(10, 200, RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/images/png/" + inImageName + ".png"), imageOptions);

		status = pdfWriter.EndPageContentContext(pageContentContext);
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

EStatusCode PNGImageTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;

	do
	{
		status = RunImageTest(inTestConfiguration, "original");
		if (status != eSuccess) {
			cout << "failed in original.png test" << "\n";
		}
		

		status = RunImageTest(inTestConfiguration, "original_transparent");
		if (status != eSuccess) {
			cout << "failed in original.png test" << "\n";
		}

		status = RunImageTest(inTestConfiguration, "gray-alpha-8-linear");
		if (status != eSuccess) {
			cout << "failed in original.png test" << "\n";
		}

		status = RunImageTest(inTestConfiguration, "gray-16-linear");
		if (status != eSuccess) {
			cout << "failed in original.png test" << "\n";
		}

		status = RunImageTest(inTestConfiguration, "pnglogo-grr");
		if (status != eSuccess) {
			cout << "failed in original.png test" << "\n";
		}

	} while (false);
	return status;
}

ADD_CATEGORIZED_TEST(PNGImageTest,"PDF Images")

#endif
