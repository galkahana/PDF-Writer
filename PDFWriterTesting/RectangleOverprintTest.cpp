/*
   Source File : RectangleOverprintTest.cpp


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
#include "DictionaryContext.h"
#include "PDFPage.h"
#include "PageContentContext.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

int RectangleOverprintTest(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(
			BuildRelativeOutputPath(argv, "RectangleOverprintTest.pdf"),
			ePDFVersion14,
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(false, true)
		);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		ObjectsContext& objCtx = pdfWriter.GetObjectsContext();

		// Create first graphics state - normal printing (no overprint)
		ObjectIDType normalGSID = objCtx.StartNewIndirectObject();
		if(normalGSID == 0)
		{
			status = PDFHummus::eFailure;
			cout << "failed to start normal graphics state object\n";
			break;
		}
		DictionaryContext* normalDict = objCtx.StartDictionary();
		normalDict->WriteKey("Type");
		normalDict->WriteNameValue("ExtGState");
		normalDict->WriteKey("OP");	// overprint for stroke
		normalDict->WriteBooleanValue(false);
		normalDict->WriteKey("op");	// overprint for fill
		normalDict->WriteBooleanValue(false);
		objCtx.EndDictionary(normalDict);
		objCtx.EndIndirectObject();

		// Create second graphics state - overprint enabled
		ObjectIDType overprintGSID = objCtx.StartNewIndirectObject();
		if(overprintGSID == 0)
		{
			status = PDFHummus::eFailure;
			cout << "failed to start overprint graphics state object\n";
			break;
		}
		DictionaryContext* overprintDict = objCtx.StartDictionary();
		overprintDict->WriteKey("Type");
		overprintDict->WriteNameValue("ExtGState");
		overprintDict->WriteKey("OP");	// overprint for stroke
		overprintDict->WriteBooleanValue(true);
		overprintDict->WriteKey("op");	// overprint for fill
		overprintDict->WriteBooleanValue(true);
		objCtx.EndDictionary(overprintDict);
		objCtx.EndIndirectObject();

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		// Add both graphics states to the page resources
		std::string normalGSName = page->GetResourcesDictionary().AddExtGStateMapping(normalGSID);
		std::string overprintGSName = page->GetResourcesDictionary().AddExtGStateMapping(overprintGSID);

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if (NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout << "failed to create content context for page\n";
			break;
		}

		// Create graphic options for filled rectangles with CMYK colors
		// Pure Cyan: C=100%, M=0%, Y=0%, K=0% = 0xFF000000
		AbstractContentContext::GraphicOptions cyanFillOptions(AbstractContentContext::eFill,
															AbstractContentContext::eCMYK,
															0xFF000000);

		// Pure Magenta: C=0%, M=100%, Y=0%, K=0% = 0x00FF0000
		AbstractContentContext::GraphicOptions magentaFillOptions(AbstractContentContext::eFill,
															AbstractContentContext::eCMYK,
															0x00FF0000);

		// Pure Yellow: C=0%, M=0%, Y=100%, K=0% = 0x0000FF00
		AbstractContentContext::GraphicOptions yellowFillOptions(AbstractContentContext::eFill,
															AbstractContentContext::eCMYK,
															0x0000FF00);

		// Draw background rectangles first with normal graphics state (no overprint)
		pageContentContext->q(); // save graphics state
		pageContentContext->gs(normalGSName); // apply normal graphics state

		// First rectangle - cyan background
		pageContentContext->DrawRectangle(100, 600, 150, 100, cyanFillOptions);

		// Second rectangle - magenta background
		pageContentContext->DrawRectangle(200, 500, 150, 100, magentaFillOptions);

		pageContentContext->Q(); // restore graphics state

		// Now draw overlapping rectangles with overprint enabled
		pageContentContext->q(); // save graphics state
		pageContentContext->gs(overprintGSName); // apply overprint graphics state

		// Third rectangle - yellow, overlapping both previous rectangles
		// This should overprint: cyan+yellow=green, magenta+yellow=red
		pageContentContext->DrawRectangle(150, 550, 150, 100, yellowFillOptions);

		pageContentContext->Q(); // restore graphics state

		// Add some explanatory text
		PDFUsedFont* font = pdfWriter.GetFontForFile(BuildRelativeInputPath(argv,"fonts/arial.ttf"));
		if(font != NULL)
		{
			AbstractContentContext::TextOptions textOptions(font, 12, AbstractContentContext::eRGB, 0x000000);
			pageContentContext->WriteText(100, 750, "Rectangle Overprint Test (CMYK)", textOptions);
			pageContentContext->WriteText(100, 735, "Cyan and magenta rectangles drawn with normal graphics state", textOptions);
			pageContentContext->WriteText(100, 720, "Yellow rectangle drawn with overprint graphics state", textOptions);
			pageContentContext->WriteText(100, 705, "Yellow should overprint: cyan+yellow=green, magenta+yellow=red", textOptions);
		}

		pdfWriter.EndPageContentContext(pageContentContext);
		pdfWriter.WritePageAndRelease(page);

		status = pdfWriter.EndPDF();

	}while(false);

	return status == eSuccess ? 0:1;
}