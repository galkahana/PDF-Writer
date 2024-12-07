/*
 Source File : CopyingAndMergingEmptyPages.cpp
 
 
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
#include "PageContentContext.h"
#include "XObjectContentContext.h"
#include "PDFPage.h"
#include "PDFFormXObject.h"
#include "PDFDocumentCopyingContext.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;


static EStatusCode PrepareSampleEmptyPDF(char* argv[], const string& inEmptyFileName)
{
    PDFWriter pdfWriter;
    EStatusCode status;
    
    do
    {
        
        status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,inEmptyFileName),ePDFVersion13);
        if(status != PDFHummus::eSuccess)
        {
            cout<<"failed to start PDF\n";
            break;
        }
        
        
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0,0,595,842));
        
        status = pdfWriter.WritePageAndRelease(page);
        if(status != PDFHummus::eSuccess)
        {
            cout<<"failed to write page\n";
            break;
        }
        status = pdfWriter.EndPDF();
        if(status != PDFHummus::eSuccess)
        {
            cout<<"failed in end PDF\n";
            break;
        }
        
    } while (false);
	
 
    return status;
}

static EStatusCode CreateFormFromEmptyPage(char* argv[], const string& inEmptyFileName)
{
	EStatusCode status;
	PDFWriter pdfWriter;
    
	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"CreateFormFromEmptyPage.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}
        
		// Create XObjects from PDF to embed
		EStatusCodeAndObjectIDTypeList result = pdfWriter.CreateFormXObjectsFromPDF(BuildRelativeOutputPath(argv,inEmptyFileName),PDFPageRange(),ePDFPageBoxMediaBox);
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to create PDF XObjects from PDF file\n";
			status = result.first;
			break;
		}
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
        
		// place the [empty] page in the top left corner of the document
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,0,421);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(result.second.front()));
		contentContext->Q();
		
		contentContext->G(0);
		contentContext->w(1);
		contentContext->re(0,421,297.5,421);
		contentContext->S();
    
        
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context\n";
			break;
		}
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page\n";
			break;
		}
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
        
	}while(false);
    
	return status;
}

static EStatusCode CreatePageFromEmptyPage(char* argv[], const string& inEmptyFileName)
{
	EStatusCode status;
    
	do
	{
	 	PDFWriter pdfWriter;
        
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"CreatePageFromEmptyPage.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}
        
		EStatusCodeAndObjectIDTypeList result;
		
        
        // append empty page
		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeOutputPath(argv,inEmptyFileName),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from empty page pdf\n";
			status = result.first;
			break;
		}
        
        // append other pages
		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"XObjectContent.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from XObjectContent.pdf\n";
			status = result.first;
			break;
		}
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
        
	}while(false);
    
	return status;
}

static EStatusCode MergeEmptyPageToPage(char* argv[], const string& inEmptyFileName)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"MergeEmptyPageToPage.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PDFUsedFont* font = pdfWriter.GetFontForFile(BuildRelativeInputPath(argv,"fonts/arial.ttf"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			break;
		}
        
		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);
        
		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj("Testing file merge");
		if(status != PDFHummus::eSuccess)
			break;
		pageContent->ET();
        
		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);
        
		PDFPageRange singlePageRange;
		singlePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singlePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));
        
		status = pdfWriter.MergePDFPagesToPage(page,BuildRelativeOutputPath(argv,inEmptyFileName),singlePageRange);
		if(status != PDFHummus::eSuccess)
			break;
        
        
		pageContent->Q();
        
		pageContent->q();
		pageContent->cm(1,0,0,1,30,500);
		pageContent->k(0,100,100,0);
		pageContent->re(0,0,200,100);
		pageContent->f();
		pageContent->Q();
        
		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != PDFHummus::eSuccess)
			break;
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
			break;
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
			break;
	}while(false);
    
	return status;
}

static EStatusCode MergeEmptyPageToForm(char* argv[], const string& inEmptyFileName)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    PDFDocumentCopyingContext* copyingContext = NULL;
    
	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"MergeEmptyPageToForm.pdf"),
                                    ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;
        
        
        // in this test we will merge 2 pages into a PDF form, and place it twice, forming a 2X2 design. amazing.
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
        
        copyingContext = pdfWriter.CreatePDFCopyingContext(BuildRelativeOutputPath(argv,inEmptyFileName));
        if(status != PDFHummus::eSuccess)
            break;
        
        // create form for two pages.
        PDFFormXObject* newFormXObject = pdfWriter.StartFormXObject(PDFRectangle(0,0,297.5,842));
		if(!newFormXObject)
		{
			status = PDFHummus::eFailure;
			break;
		}
        
		XObjectContentContext* xobjectContentContext = newFormXObject->GetContentContext();
        
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



int CopyingAndMergingEmptyPages(int argc, char* argv[])
{
    EStatusCode status = eSuccess;
    string emptyFileName = "SampleEmptyFileForCopying.pdf";
    
    do
    {
        
        status = PrepareSampleEmptyPDF(argv,emptyFileName);
        if(status != eSuccess)
        {
            cout<<"CopyingAndMergingEmptyPages::Run, failed in creating sample empty file name for merging\n";
            break;
        }
        
        status = CreateFormFromEmptyPage(argv,emptyFileName);
        if(status != eSuccess)
        {
            cout<<"CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";
            break;
        }
        
        status = CreatePageFromEmptyPage(argv,emptyFileName);
        if(status != eSuccess)
        {
            cout<<"CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";
            break;
        }
        
        status = MergeEmptyPageToPage(argv,emptyFileName);
        if(status != eSuccess)
        {
            cout<<"CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";
            break;
        }
        
        status = MergeEmptyPageToForm(argv,emptyFileName);
        if(status != eSuccess)
        {
            cout<<"CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";
            break;
        }
        
    } while (false);
    

    return status == eSuccess ? 0:1;
}

