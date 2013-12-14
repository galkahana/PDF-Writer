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
#include "PageModifierTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFModifiedPage.h"
#include "AbstractContentContext.h"

#include <iostream>


using namespace PDFHummus;


PageModifierTest::PageModifierTest(void)
{
}


PageModifierTest::~PageModifierTest(void)
{
}


EStatusCode PageModifierTest::Run(const TestConfiguration& inTestConfiguration)
{

    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;
    
    do 
    {
        
        // Modify existing page. first modify it's page box, then anoteher page content

 		// open file for modification
        status = pdfWriter.ModifyPDF(
                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("TestMaterials/XObjectContent.pdf")),
                                     ePDFVersion13,
                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("XObjectContentModified.pdf")),
                                     LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("XObjectContentModified.log"))));  
		if(status != eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	        

		PDFModifiedPage modifiedPage(&pdfWriter,0);

		AbstractContentContext* contentContext = modifiedPage.StartContentContext();
		AbstractContentContext::TextOptions opt(
			pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,
                                                                            "TestMaterials/fonts/arial.ttf")),
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
    
    return status;
}

ADD_CATEGORIZED_TEST(PageModifierTest,"Modification")