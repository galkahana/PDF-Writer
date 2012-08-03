/*
 Source File : MergeToPDFForm.cpp
 
 
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
#include "MergeToPDFForm.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "XObjectContentContext.h"
#include "PDFPage.h"
#include "PDFFormXObject.h"
#include "PDFDocumentCopyingContext.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

MergeToPDFForm::MergeToPDFForm(void)
{
}

MergeToPDFForm::~MergeToPDFForm(void)
{
}

EStatusCode MergeToPDFForm::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    PDFDocumentCopyingContext* copyingContext = NULL;
    
	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"MergeToPDFForm.pdf"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"MergeToPDFForm.txt")));
		if(status != PDFHummus::eSuccess)
			break;
        
        
        // in this test we will merge 2 pages into a PDF form, and place it twice, forming a 2X2 design. amazing.
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
        
        copyingContext = pdfWriter.CreatePDFCopyingContext(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/Linearized.pdf"));
        if(status != PDFHummus::eSuccess)
            break;
        
        // create form for two pages.
        PDFFormXObject* newFormXObject = pdfWriter.StartFormXObject(PDFRectangle(0,0,297.5,842));
        
		XObjectContentContext* xobjectContentContext = newFormXObject->GetContentContext();
        
        xobjectContentContext->q();
        xobjectContentContext->cm(0.5,0,0,0.5,0,0);
        status = copyingContext->MergePDFPageToFormXObject(newFormXObject,1);
        if(status != eSuccess)
            break;
        xobjectContentContext->Q();
        
        xobjectContentContext->q();
        xobjectContentContext->cm(0.5,0,0,0.5,0,421);
        status = copyingContext->MergePDFPageToFormXObject(newFormXObject,0);
        if(status != eSuccess)
            break;
        xobjectContentContext->Q();
        
        ObjectIDType formID = newFormXObject->GetObjectID();
        status = pdfWriter.EndFormXObjectAndRelease(newFormXObject);
        if(status != eSuccess)
            break;
        
        // now place it in the page
        PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
        
        
        string formName = page->GetResourcesDictionary().AddFormXObjectMapping(formID);
  
        pageContentContext->q();
        pageContentContext->Do(formName);
        pageContentContext->cm(1,0,0,1,297.5,0);
        pageContentContext->Do(formName);
        pageContentContext->Q();
        
        status = pdfWriter.EndPageContentContext(pageContentContext);
        if(status != eSuccess)
            break;
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
			break;
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
			break;
	}while(false);
    
    delete copyingContext;
	
	return status;
}



ADD_CATEGORIZED_TEST(MergeToPDFForm,"PDFEmbedding")
