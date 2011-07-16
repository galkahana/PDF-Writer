/*
   Source File : MergePDFPages.cpp


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
#include "MergePDFPages.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "PDFPage.h"
#include "DocumentContextExtenderAdapter.h"
#include "PDFDocumentCopyingContext.h"

#include <iostream>

using namespace std;

MergePDFPages::MergePDFPages(void)
{
}

MergePDFPages::~MergePDFPages(void)
{
}

EStatusCode MergePDFPages::Run()
{
	EStatusCode status = eSuccess;

/*
a. Regular (no copying context):
	a. Merging one page
		0. No graphics
		i. Some graphic before
		ii. Some graphic after
		iii. Both before and after
	b. Merging two pages
		i. In two phases
			1) Some graphics between them, and after the last one
		ii. In one phase, with events for some graphics between them
b. Copying context:
	a. Simple, merging one page with some graphics before and after

Importing two pages, merge one and user the other as xobject, with graphics between them
*/

	if(TestOnlyMerge() != eSuccess)
	{
		wcout<<"Failed [TestOnlyMerge()]: test for clean merging of one page \n";
		status = eFailure;
	}

	if(TestPrefixGraphicsMerge() != eSuccess)
	{
		wcout<<"Failed [TestPrefixGraphicsMerge()]: test for merging of one page with some prior graphics \n";
		status = eFailure;
	}

	if(TestSuffixGraphicsMerge() != eSuccess)
	{
		wcout<<"Failed [TestSuffixGraphicsMerge()]: test for merging of one page with some graphics after the merged page\n";
		status = eFailure;
	}

	if(TestBothGraphicsMerge() != eSuccess)
	{
		wcout<<"Failed [TestBothGraphicsMerge()]: test for merging of one page with some graphics before and after the merged page\n";
		status = eFailure;
	}

	if(MergeTwoPageInSeparatePhases() != eSuccess)
	{
		wcout<<"Failed [MergeTwoPageInSeparatePhases()]: test for merging of two pages in two separate pheases with some graphics between them\n";
		status = eFailure;
	}

	if(MergeTwoPageWithEvents() != eSuccess)
	{
		wcout<<"Failed [MergeTwoPageWithEvents()]: test for merging of two pages with some graphics between them, using the events system\n";
		status = eFailure;
	}

	if(MergePagesUsingCopyingContext() != eSuccess)
	{
		wcout<<"Failed [MergeTwoPageUsingCopyingContext()]: test for merging of multiple pages with some graphics between them and also using some as xobjects, using the copying contexts\n";
		status = eFailure;
	}
	return status;
}

static const wstring scBasePath = L"c:\\PDFLibTests\\";
static const wstring scMergeFilePath = L"c:\\PDFLibTests\\TestMaterials\\BasicTIFFImagesTest.PDF";
static const wstring scFontPath = L"C:\\PDFLibTests\\TestMaterials\\fonts\\arial.ttf";

EStatusCode MergePDFPages::TestOnlyMerge()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"TestOnlyMerge.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	
	return status;
}

EStatusCode MergePDFPages::TestPrefixGraphicsMerge()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"TestPrefixGraphicsMerge.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFUsedFont* font = pdfWriter.GetFontForFile(scFontPath);
		if(!font)
		{
			status = eFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj(L"Testing file merge");
		if(status != eSuccess)
			break;
		pageContent->ET();

		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	
	return status;
}

EStatusCode MergePDFPages::TestSuffixGraphicsMerge()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"TestSuffixGraphicsMerge.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != eSuccess)
			break;

		PDFUsedFont* font = pdfWriter.GetFontForFile(scFontPath);
		if(!font)
		{
			status = eFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj(L"Testing file merge");
		if(status != eSuccess)
			break;
		pageContent->ET();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	
	return status;
}

EStatusCode MergePDFPages::TestBothGraphicsMerge()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"TestBothGraphicsMerge.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFUsedFont* font = pdfWriter.GetFontForFile(scFontPath);
		if(!font)
		{
			status = eFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj(L"Testing file merge");
		if(status != eSuccess)
			break;
		pageContent->ET();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != eSuccess)
			break;


		pageContent->Q();

		pageContent->q();
		pageContent->cm(1,0,0,1,30,500);
		pageContent->k(0,100,100,0);
		pageContent->re(0,0,200,100);
		pageContent->f();
		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	return status;
}

EStatusCode MergePDFPages::MergeTwoPageInSeparatePhases()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"MergeTwoPageInSeparatePhases.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange firstPageRange;
		firstPageRange.mType = PDFPageRange::eRangeTypeSpecific;
		firstPageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,firstPageRange);
		if(status != eSuccess)
			break;

		pageContent->Q();


		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,421);

		PDFPageRange secondPageRange;
		secondPageRange.mType = PDFPageRange::eRangeTypeSpecific;
		secondPageRange.mSpecificRanges.push_back(ULongAndULong(1,1));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,secondPageRange);
		if(status != eSuccess)
			break;

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	
	return status;
}

// for the next trick i'll need an event handler, in order to write the code between the pages.
class MyPDFMergingHandler : public DocumentContextExtenderAdapter
{
public:
	MyPDFMergingHandler(PageContentContext* inPageContentContext) {mPageContentContext = inPageContentContext; mPageIndex = 0;}

	EStatusCode OnAfterMergePageFromPage(
					PDFPage* inTargetPage,
					PDFDictionary* inPageObjectDictionary,
					ObjectsContext* inPDFWriterObjectContext,
					DocumentContext* inPDFWriterDocumentContext,
					PDFDocumentHandler* inPDFDocumentHandler)
	{
		// implementing the after merge for page event to include the required code between the two pages

		if(0 == mPageIndex)
		{
			mPageContentContext->Q();
			mPageContentContext->q();
			mPageContentContext->cm(0.5,0,0,0.5,0,421);
		}
		++mPageIndex;
		return eSuccess;
	}

private:
	int mPageIndex;
	PageContentContext* mPageContentContext;

};


EStatusCode MergePDFPages::MergeTwoPageWithEvents()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"MergeTwoPageWithEvents.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange twoPageRange;
		twoPageRange.mType = PDFPageRange::eRangeTypeSpecific;
		twoPageRange.mSpecificRanges.push_back(ULongAndULong(0,1));

		MyPDFMergingHandler mergingHandler(pageContent);

		// i'm using events to write the interim code, as oppose to using two merges as TestBothGraphicsMerge shows. this is more efficient, considering the embedded PDF. 
		// but still, the easiest would be to use the copying context
		pdfWriter.GetDocumentContext().AddDocumentContextExtender(&mergingHandler);
		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,twoPageRange);
		if(status != eSuccess)
			break;
		pdfWriter.GetDocumentContext().RemoveDocumentContextExtender(&mergingHandler);

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	return status;
}

EStatusCode MergePDFPages::MergePagesUsingCopyingContext()
{
	// this is by far the best method if you want to merge multiple pages in a pdf - using the copying context
	PDFWriter pdfWriter;
	EStatusCode status;

	// in this sample we'll create two pages, from 3 pages content of the merged page.
	// the first page will be used as a reusable object in both result pages. the second and third page will 
	// be reusables.

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"MergePagesUsingCopyingContext.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFDocumentCopyingContext* copyingContext = pdfWriter.CreatePDFCopyingContext(scMergeFilePath);
		if(!copyingContext)
		{
			status = eFailure;
			break;
		}
		
		// create a reusable form xobject from the first page
		EStatusCodeAndObjectIDType result = copyingContext->CreateFormXObjectFromPDFPage(0,ePDFPageBoxMediaBox);
		if(result.first != eSuccess)
		{
			status = eFailure;
			break;
		}
		ObjectIDType reusableObjectID = result.second;

		// now let's begin constructing the pages

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		// merge unique page at lower left
		status = copyingContext->MergePDFPageToPage(page,1);
		if(status != eSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		// now let's do the second page. similar, but with the second page as the unique content

		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		// merge unique page at lower left
		status = copyingContext->MergePDFPageToPage(page,2);
		if(status != eSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	return eSuccess;
}



ADD_CATEGORIZED_TEST(MergePDFPages,"PDFEmbedding")
