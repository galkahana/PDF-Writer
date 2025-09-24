/*
 Source File : PDFOutlinesTest.cpp


 Copyright 2025 Gal Kahana PDFWriter

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
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFUsedFont.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "DocumentContextExtenderAdapter.h"
#include "AbstractContentContext.h"

#include <iostream>

#include "testing/TestIO.h"
#include "OutlineBuilder.h"

using namespace std;
using namespace PDFHummus;



class OutlinesExtender : public DocumentContextExtenderAdapter
{
public:
    OutlinesExtender(ObjectIDType inOutlinesObjectId) : mOutlinesObjectId(inOutlinesObjectId)
    {
    }

    virtual EStatusCode OnCatalogWrite(
        CatalogInformation* inCatalogInformation,
        DictionaryContext* inCatalogDictionaryContext,
        ObjectsContext* inPDFWriterObjectContext,
        DocumentContext* inDocumentContext)
    {
        if(mOutlinesObjectId != 0)
        {
            inCatalogDictionaryContext->WriteKey("Outlines");
            inCatalogDictionaryContext->WriteObjectReferenceValue(mOutlinesObjectId);
        }
        return eSuccess;
    }

private:
    ObjectIDType mOutlinesObjectId;
};

static EStatusCodeAndObjectIDType CreatePageWithContent(PDFWriter* inPDFWriter, PDFUsedFont* inFont, const string& inTitle, const string& inContent)
{
    EStatusCode status = eSuccess;
    ObjectIDType pageId = 0;

    do
    {
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* pageContentContext = inPDFWriter->StartPageContentContext(page);
        if(!pageContentContext)
        {
            status = eFailure;
            cout << "failed to create page content context\n";
            break;
        }

        AbstractContentContext::TextOptions titleOptions(inFont, 24, AbstractContentContext::eGray, 0);
        AbstractContentContext::TextOptions contentOptions(inFont, 12, AbstractContentContext::eGray, 0);

        pageContentContext->WriteText(100, 750, inTitle, titleOptions);
        pageContentContext->WriteText(50, 700, inContent, contentOptions);

        status = inPDFWriter->EndPageContentContext(pageContentContext);
        if(status != eSuccess)
        {
            cout << "failed to end page content context\n";
            break;
        }

        EStatusCodeAndObjectIDType pageResult = inPDFWriter->WritePageAndReturnPageID(page);
        if(pageResult.first != eSuccess)
        {
            cout << "failed to write page\n";
            status = eFailure;
            break;
        }

        pageId = pageResult.second;
    }
    while(false);

    return EStatusCodeAndObjectIDType(status, pageId);
}

int PDFOutlinesTest(int argc, char* argv[])
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    do
    {
        status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv, "PDFWithOutlines.pdf"), ePDFVersion13);
        if(status != eSuccess)
        {
            cout << "failed to start PDF\n";
            break;
        }

        PDFUsedFont* font = pdfWriter.GetFontForFile(BuildRelativeInputPath(argv, "fonts/arial.ttf"));
        if(!font)
        {
            status = eFailure;
            cout << "failed to create font\n";
            break;
        }

        // Create pages - some with outlines, some without
        EStatusCodeAndObjectIDType pageResult;

        // Page 1: Chapter 1
        pageResult = CreatePageWithContent(&pdfWriter, font, "Chapter 1: Introduction", "This is the introduction chapter of our document.");
        if(pageResult.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        ObjectIDType page1Id = pageResult.second;

        // Page 2: Chapter 2 Section A
        pageResult = CreatePageWithContent(&pdfWriter, font, "Chapter 2: Section A", "This covers section A of chapter 2.");
        if(pageResult.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        ObjectIDType page2Id = pageResult.second;

        // Page 3: Chapter 2 Section B
        pageResult = CreatePageWithContent(&pdfWriter, font, "Chapter 2: Section B", "This covers section B of chapter 2.");
        if(pageResult.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        ObjectIDType page3Id = pageResult.second;

        // Page 4: No outline - just content
        pageResult = CreatePageWithContent(&pdfWriter, font, "Additional Content", "This page has content but no outline entry.");
        if(pageResult.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        // Note: We don't use this page ID in any outline

        // Page 5: Chapter 3
        pageResult = CreatePageWithContent(&pdfWriter, font, "Chapter 3: Conclusion", "This chapter provides the conclusion and summary.");
        if(pageResult.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        ObjectIDType page5Id = pageResult.second;

        // Create nested outline structure
        OutlineItem chapter2Children[2];
        chapter2Children[0] = OutlineItem("Section A", page2Id);
        chapter2Children[1] = OutlineItem("Section B", page3Id);

        OutlineItem outlineItems[3];
        outlineItems[0] = OutlineItem("Chapter 1: Introduction", page1Id);
        outlineItems[1] = OutlineItem("Chapter 2: Implementation", page2Id, chapter2Children, 2);
        outlineItems[2] = OutlineItem("Chapter 3: Conclusion", page5Id);

        // Create outline tree and get the outline object ID
        ObjectIDType outlinesObjectId;
        OutlineBuilder outlineBuilder;
        status = outlineBuilder.CreateOutlineTree(&pdfWriter.GetObjectsContext(), outlineItems, 3, 842.0, outlinesObjectId);
        if(status != eSuccess)
        {
            cout << "failed to write outline tree\n";
            break;
        }

        // Set up extender to reference the outline in catalog
        OutlinesExtender outlinesExtender(outlinesObjectId);
        pdfWriter.GetDocumentContext().AddDocumentContextExtender(&outlinesExtender);

        status = pdfWriter.EndPDF();
        if(status != eSuccess)
        {
            cout << "failed in end PDF\n";
            break;
        }
    }
    while(false);

    return status == eSuccess ? 0:1;
}