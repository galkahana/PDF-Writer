/*
   Source File : WatermarkTest.cpp


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

int WatermarkTest(int argc, char* argv[])
{
    PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(
            BuildRelativeOutputPath(argv, "WatermarkTest.pdf"), 
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
        ObjectIDType gsID = objCtx.StartNewIndirectObject();
        if(gsID == 0)
        {
            status = PDFHummus::eFailure;
            cout << "failed to start extgstate object\n";
            break;
        }
        DictionaryContext* dict = objCtx.StartDictionary();
        dict->WriteKey("type");
        dict->WriteNameValue("ExtGState");
        dict->WriteKey("ca");
        dict->WriteDoubleValue(0.5);
        objCtx.EndDictionary(dict);
        objCtx.EndIndirectObject();
       
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0,0,595,842));
        std::string gsName = page->GetResourcesDictionary().AddExtGStateMapping(gsID);
        
        PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if (NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout << "failed to create content context for page\n";
            break;
		}


        // background so we can see the transparency
		AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill,
																AbstractContentContext::eCMYK,
																0xFF00FF00);        
        pageContentContext->DrawRectangle(50,400,200,200,pathFillOptions);

        // some transparent text
        pageContentContext->q();
        pageContentContext->gs(gsName);
        pageContentContext->BT();
        pageContentContext->k(0, 100, 100, 0);
        PDFUsedFont* font = pdfWriter.GetFontForFile(BuildRelativeInputPath(argv,"fonts/arial.ttf"));
        pageContentContext->Tf(font, 30);
        pageContentContext->Tm(1,0,0,1,100,500);
        pageContentContext->Tj("Test Text");
        pageContentContext->ET();

        // some more text, using higher level commands
		AbstractContentContext::TextOptions textOptions(font,14,AbstractContentContext::eCMYK,0x00FFFF00);        
        pageContentContext->WriteText(100,550,"some more text", textOptions);

        pageContentContext->Q();

        pdfWriter.EndPageContentContext(pageContentContext);
        pdfWriter.WritePageAndRelease(page);
        pdfWriter.EndPDF();

	}while(false);

	return status == eSuccess ? 0:1;	
}
