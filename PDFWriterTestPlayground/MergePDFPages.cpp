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
using namespace PDFHummus;

MergePDFPages::MergePDFPages(void)
{
}

MergePDFPages::~MergePDFPages(void)
{
}

EStatusCode MergePDFPages::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;

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

	if(TestOnlyMerge(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [TestOnlyMerge()]: test for clean merging of one page \n";
		status = PDFHummus::eFailure;
	}

	if(TestPrefixGraphicsMerge(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [TestPrefixGraphicsMerge()]: test for merging of one page with some prior graphics \n";
		status = PDFHummus::eFailure;
	}

	if(TestSuffixGraphicsMerge(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [TestSuffixGraphicsMerge()]: test for merging of one page with some graphics after the merged page\n";
		status = PDFHummus::eFailure;
	}

	if(TestBothGraphicsMerge(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [TestBothGraphicsMerge()]: test for merging of one page with some graphics before and after the merged page\n";
		status = PDFHummus::eFailure;
	}

	if(MergeTwoPageInSeparatePhases(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [MergeTwoPageInSeparatePhases()]: test for merging of two pages in two separate pheases with some graphics between them\n";
		status = PDFHummus::eFailure;
	}

	if(MergeTwoPageWithEvents(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [MergeTwoPageWithEvents()]: test for merging of two pages with some graphics between them, using the events system\n";
		status = PDFHummus::eFailure;
	}

	if(MergePagesUsingCopyingContext(inTestConfiguration) != PDFHummus::eSuccess)
	{
		cout<<"Failed [MergeTwoPageUsingCopyingContext()]: test for merging of multiple pages with some graphics between them and also using some as xobjects, using the copying contexts\n";
		status = PDFHummus::eFailure;
	}
	return status;
}

EStatusCode MergePDFPages::TestOnlyMerge(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestOnlyMerge.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,
                                               RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),singePageRange);
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

EStatusCode MergePDFPages::TestPrefixGraphicsMerge(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestPrefixGraphicsMerge.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFUsedFont* font = pdfWriter.GetFontForFile(
                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
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

		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),singePageRange);
		if(status != PDFHummus::eSuccess)
			break;

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

EStatusCode MergePDFPages::TestSuffixGraphicsMerge(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestSuffixGraphicsMerge.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),singePageRange);
		if(status != PDFHummus::eSuccess)
			break;

		PDFUsedFont* font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
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

EStatusCode MergePDFPages::TestBothGraphicsMerge(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestBothGraphicsMerge.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFUsedFont* font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
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

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),singePageRange);
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

EStatusCode MergePDFPages::MergeTwoPageInSeparatePhases(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"MergeTwoPageInSeparatePhases.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		PDFPageRange firstPageRange;
		firstPageRange.mType = PDFPageRange::eRangeTypeSpecific;
		firstPageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),firstPageRange);
		if(status != PDFHummus::eSuccess)
			break;

		pageContent->Q();


		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,421);

		PDFPageRange secondPageRange;
		secondPageRange.mType = PDFPageRange::eRangeTypeSpecific;
		secondPageRange.mSpecificRanges.push_back(ULongAndULong(1,1));

		status = pdfWriter.MergePDFPagesToPage(page,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),secondPageRange);
		if(status != PDFHummus::eSuccess)
			break;

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
		return PDFHummus::eSuccess;
	}

private:
	int mPageIndex;
	PageContentContext* mPageContentContext;

};


EStatusCode MergePDFPages::MergeTwoPageWithEvents(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"MergeTwoPageWithEvents.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
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
		status = pdfWriter.MergePDFPagesToPage(page,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"),twoPageRange);
		if(status != PDFHummus::eSuccess)
			break;
		pdfWriter.GetDocumentContext().RemoveDocumentContextExtender(&mergingHandler);

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

EStatusCode MergePDFPages::MergePagesUsingCopyingContext(const TestConfiguration& inTestConfiguration)
{
	// this is by far the best method if you want to merge multiple pages in a pdf - using the copying context
	PDFWriter pdfWriter;
	EStatusCode status;

	// in this sample we'll create two pages, from 3 pages content of the merged page.
	// the first page will be used as a reusable object in both result pages. the second and third page will 
	// be reusables.

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"MergePagesUsingCopyingContext.pdf"),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
			break;

		PDFDocumentCopyingContext* copyingContext = pdfWriter.CreatePDFCopyingContext(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"));
		if(!copyingContext)
		{
			status = PDFHummus::eFailure;
			break;
		}
		
		// create a reusable form xobject from the first page
		EStatusCodeAndObjectIDType result = copyingContext->CreateFormXObjectFromPDFPage(0,ePDFPageBoxMediaBox);
		if(result.first != PDFHummus::eSuccess)
		{
			status = PDFHummus::eFailure;
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
		if(status != PDFHummus::eSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != PDFHummus::eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
			break;

		// now let's do the second page. similar, but with the second page as the unique content

		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		// merge unique page at lower left
		status = copyingContext->MergePDFPageToPage(page,2);
		if(status != PDFHummus::eSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

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

	return PDFHummus::eSuccess;
}



ADD_CATEGORIZED_TEST(MergePDFPages,"PDFEmbedding")
