/*
 Source File : PageModifierTest.cpp
 
 
 Copyright 2013 Gal Kahana PDFWriter
 
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

#include <iostream>

#include "testing/TestIO.h"


using namespace PDFHummus;


int PageModifierTest(int argc, char* argv[])
{

    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;
    
    do 
    {
        
        // Modify existing page. first modify it's page box, then anoteher page content

 		// open file for modification
        status = pdfWriter.ModifyPDF(
                                     BuildRelativeInputPath(argv,string("XObjectContent.pdf")),
                                     ePDFVersion13,
                                     BuildRelativeOutputPath(argv,string("XObjectContentModified.pdf")),
                                     LogConfiguration(true,true,BuildRelativeOutputPath(argv,string("XObjectContentModified.log"))));  
		if(status != eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	        

		PDFModifiedPage modifiedPage(&pdfWriter,0);

		AbstractContentContext* contentContext = modifiedPage.StartContentContext();
		AbstractContentContext::TextOptions opt(
			pdfWriter.GetFontForFile(BuildRelativeInputPath(argv,
															"fonts/arial.ttf")),
															14,
															AbstractContentContext::eGray,
															0
			);

		contentContext->WriteText(75,805,"Test Text",opt);

		modifiedPage.EndContentContext();
		modifiedPage.WritePage();

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
    }
    while(false);
    
    return status == eSuccess ? 0:1;
}
