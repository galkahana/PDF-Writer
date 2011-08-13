/*
   Source File : MergePDFPages.cpp


   Copyright 2011 Gal Kahana HummusPDFWriter

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
#include "HummusPDFWriter.h"
#include "PageContentContext.h"
#include "PDFPage.h"
#include "DocumentsContextExtenderAdapter.h"
#include "PDFDocumentCopyingContext.h"

#include <iostream>

using namespace std;

MergePDFPages::MergePDFPages(void)
{
}

MergePDFPages::~MergePDFPages(void)
{
}

EPDFStatusCode MergePDFPages::Run()
{
	EPDFStatusCode status = ePDFSuccess;

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

	if(TestOnlyMerge() != ePDFSuccess)
	{
		cout<<"Failed [TestOnlyMerge()]: test for clean merging of one page \n";
		status = ePDFFailure;
	}

	if(TestPrefixGraphicsMerge() != ePDFSuccess)
	{
		cout<<"Failed [TestPrefixGraphicsMerge()]: test for merging of one page with some prior graphics \n";
		status = ePDFFailure;
	}

	if(TestSuffixGraphicsMerge() != ePDFSuccess)
	{
		cout<<"Failed [TestSuffixGraphicsMerge()]: test for merging of one page with some graphics after the merged page\n";
		status = ePDFFailure;
	}

	if(TestBothGraphicsMerge() != ePDFSuccess)
	{
		cout<<"Failed [TestBothGraphicsMerge()]: test for merging of one page with some graphics before and after the merged page\n";
		status = ePDFFailure;
	}

	if(MergeTwoPageInSeparatePhases() != ePDFSuccess)
	{
		cout<<"Failed [MergeTwoPageInSeparatePhases()]: test for merging of two pages in two separate pheases with some graphics between them\n";
		status = ePDFFailure;
	}

	if(MergeTwoPageWithEvents() != ePDFSuccess)
	{
		cout<<"Failed [MergeTwoPageWithEvents()]: test for merging of two pages with some graphics between them, using the events system\n";
		status = ePDFFailure;
	}

	if(MergePagesUsingCopyingContext() != ePDFSuccess)
	{
		cout<<"Failed [MergeTwoPageUsingCopyingContext()]: test for merging of multiple pages with some graphics between them and also using some as xobjects, using the copying contexts\n";
		status = ePDFFailure;
	}
	return status;
}

static const string scBasePath = "c:\\PDFLibTests\\";
static const string scMergeFilePath = "c:\\PDFLibTests\\TestMaterials\\BasicTIFFImagesTest.PDF";
static const string scFontPath = "C:\\PDFLibTests\\TestMaterials\\fonts\\arial.ttf";

EPDFStatusCode MergePDFPages::TestOnlyMerge()
{
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "TestOnlyMerge.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	
	return status;
}

EPDFStatusCode MergePDFPages::TestPrefixGraphicsMerge()
{
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "TestPrefixGraphicsMerge.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFUsedFont* font = pdfWriter.GetFontForFile(scFontPath);
		if(!font)
		{
			status = ePDFFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj("Testing file merge");
		if(status != ePDFSuccess)
			break;
		pageContent->ET();

		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	
	return status;
}

EPDFStatusCode MergePDFPages::TestSuffixGraphicsMerge()
{
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "TestSuffixGraphicsMerge.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != ePDFSuccess)
			break;

		PDFUsedFont* font = pdfWriter.GetFontForFile(scFontPath);
		if(!font)
		{
			status = ePDFFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj("Testing file merge");
		if(status != ePDFSuccess)
			break;
		pageContent->ET();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	
	return status;
}

EPDFStatusCode MergePDFPages::TestBothGraphicsMerge()
{
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "TestBothGraphicsMerge.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFUsedFont* font = pdfWriter.GetFontForFile(scFontPath);
		if(!font)
		{
			status = ePDFFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,0,1);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,600);
		status = pageContent->Tj("Testing file merge");
		if(status != ePDFSuccess)
			break;
		pageContent->ET();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,singePageRange);
		if(status != ePDFSuccess)
			break;


		pageContent->Q();

		pageContent->q();
		pageContent->cm(1,0,0,1,30,500);
		pageContent->k(0,100,100,0);
		pageContent->re(0,0,200,100);
		pageContent->f();
		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	return status;
}

EPDFStatusCode MergePDFPages::MergeTwoPageInSeparatePhases()
{
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "MergeTwoPageInSeparatePhases.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
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
		if(status != ePDFSuccess)
			break;

		pageContent->Q();


		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,421);

		PDFPageRange secondPageRange;
		secondPageRange.mType = PDFPageRange::eRangeTypeSpecific;
		secondPageRange.mSpecificRanges.push_back(ULongAndULong(1,1));

		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,secondPageRange);
		if(status != ePDFSuccess)
			break;

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	
	return status;
}

// for the next trick i'll need an event handler, in order to write the code between the pages.
class MyPDFMergingHandler : public DocumentsContextExtenderAdapter
{
public:
	MyPDFMergingHandler(PageContentContext* inPageContentContext) {mPageContentContext = inPageContentContext; mPageIndex = 0;}

	EPDFStatusCode OnAfterMergePageFromPage(
					PDFPage* inTargetPage,
					PDFDictionary* inPageObjectDictionary,
					ObjectsContext* inHummusPDFWriterObjectContext,
					DocumentsContext* inHummusPDFWriterDocumentsContext,
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
		return ePDFSuccess;
	}

private:
	int mPageIndex;
	PageContentContext* mPageContentContext;

};


EPDFStatusCode MergePDFPages::MergeTwoPageWithEvents()
{
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "MergeTwoPageWithEvents.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
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
		pdfWriter.GetDocumentsContext().AddDocumentsContextExtender(&mergingHandler);
		status = pdfWriter.MergePDFPagesToPage(page,scMergeFilePath,twoPageRange);
		if(status != ePDFSuccess)
			break;
		pdfWriter.GetDocumentsContext().RemoveDocumentsContextExtender(&mergingHandler);

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	return status;
}

EPDFStatusCode MergePDFPages::MergePagesUsingCopyingContext()
{
	// this is by far the best method if you want to merge multiple pages in a pdf - using the copying context
	HummusPDFWriter pdfWriter;
	EPDFStatusCode status;

	// in this sample we'll create two pages, from 3 pages content of the merged page.
	// the first page will be used as a reusable object in both result pages. the second and third page will 
	// be reusables.

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "MergePagesUsingCopyingContext.pdf",ePDFVersion13);
		if(status != ePDFSuccess)
			break;

		PDFDocumentCopyingContext* copyingContext = pdfWriter.CreatePDFCopyingContext(scMergeFilePath);
		if(!copyingContext)
		{
			status = ePDFFailure;
			break;
		}
		
		// create a reusable form xobject from the first page
		EPDFStatusCodeAndObjectIDType result = copyingContext->CreateFormXObjectFromPDFPage(0,ePDFPageBoxMediaBox);
		if(result.first != ePDFSuccess)
		{
			status = ePDFFailure;
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
		if(status != ePDFSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		// now let's do the second page. similar, but with the second page as the unique content

		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		// merge unique page at lower left
		status = copyingContext->MergePDFPageToPage(page,2);
		if(status != ePDFSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != ePDFSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != ePDFSuccess)
			break;
	}while(false);

	return ePDFSuccess;
}



ADD_CATEGORIZED_TEST(MergePDFPages,"PDFEmbedding")
