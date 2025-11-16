/*
   Source File : TableOfContentsTest.cpp

   Example demonstrating how to create a table of contents with clickable page links
   using PDF's indirect object pattern for forward references.

   This example creates:
   - A title page (page 1)
   - A table of contents page with clickable links to sections (page 2)
   - Multiple content pages representing different sections (pages 3-7)

   The TOC links use PDF GoTo actions with explicit destinations to jump to specific pages.

   TECHNIQUE: Forward References with Indirect Objects
   To ensure the TOC appears as page 2 (not the last page), this example demonstrates:
   1. Pre-allocating annotation object IDs before writing the TOC page
   2. Writing the TOC page with references to those pre-allocated IDs
   3. Creating content pages and capturing their page IDs
   4. Writing the annotation object bodies with the correct page destinations

   This allows the TOC page to reference annotations that haven't been fully written yet,
   leveraging PDF's indirect object mechanism to establish forward references.

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
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "DocumentContext.h"
#include "PDFUsedFont.h"

#include "testing/TestIO.h"

#include <string>
#include <vector>
#include <cstdio>

using namespace PDFHummus;

// Structure to hold TOC entry information
struct TOCEntry
{
    std::string title;
    ObjectIDType annotationID;  // Pre-allocated annotation object ID
    ObjectIDType pageID;        // Target content page ID (filled in later)
    PDFRectangle linkRect;      // Clickable rectangle on TOC page
    double yPosition;           // Y position on TOC page for this entry
    int pageNumber;             // Page number to display in TOC
};

// Helper function to write an internal page link annotation body with a pre-allocated object ID
// This creates a Link annotation with a GoTo action pointing to a specific page
// Used in the indirect object pattern where the object ID is allocated before the body is written
void WriteInternalPageLinkBody(
    PDFWriter& pdfWriter,
    ObjectIDType inAnnotationID,
    const PDFRectangle& inLinkClickArea,
    ObjectIDType inTargetPageID)
{
    ObjectsContext& objectsContext = pdfWriter.GetObjectsContext();

    // Start writing the annotation object with the pre-allocated ID
    objectsContext.StartNewIndirectObject(inAnnotationID);

    DictionaryContext* annotationDict = objectsContext.StartDictionary();

    // Type: Annotation
    annotationDict->WriteKey("Type");
    annotationDict->WriteNameValue("Annot");

    // Subtype: Link
    annotationDict->WriteKey("Subtype");
    annotationDict->WriteNameValue("Link");

    // Rect: The clickable area
    annotationDict->WriteKey("Rect");
    annotationDict->WriteRectangleValue(inLinkClickArea);

    // F: Flags (4 = Print flag, so link area is visible when printed)
    annotationDict->WriteKey("F");
    annotationDict->WriteIntegerValue(4);

    // BS: Border Style (width 0 = no visible border)
    annotationDict->WriteKey("BS");
    DictionaryContext* borderStyle = objectsContext.StartDictionary();
    borderStyle->WriteKey("W");
    borderStyle->WriteIntegerValue(0);
    objectsContext.EndDictionary(borderStyle);

    // Dest: Explicit destination array [page /Fit]
    // This tells the PDF viewer to go to the specified page and fit it to the window
    annotationDict->WriteKey("Dest");
    objectsContext.StartArray();
    objectsContext.WriteIndirectObjectReference(inTargetPageID);
    objectsContext.WriteName("Fit");
    objectsContext.EndArray();

    objectsContext.EndDictionary(annotationDict);
    objectsContext.EndIndirectObject();
}

// Helper function to write text at a specific position
void WriteTextAt(PageContentContext* context, PDFUsedFont* font, double x, double y,
                 const std::string& text, double fontSize = 12)
{
    AbstractContentContext::TextOptions textOptions(font, fontSize,
                                                     AbstractContentContext::eGray, 0);
    context->WriteText(x, y, text, textOptions);
}

// Helper function to add a centered page number at the bottom of a page
void AddPageNumber(PageContentContext* context, PDFUsedFont* font, int pageNumber)
{
    const double pageWidth = 595;
    const double bottomMargin = 30;
    const double fontSize = 10;

    // Create page number text (e.g., "Page 3" or just "3")
    std::string pageText = "Page ";
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", pageNumber);
    pageText += buffer;

    // Calculate text width for centering
    double textWidth = font->CalculateTextAdvance(pageText, fontSize);
    double x = (pageWidth - textWidth) / 2.0;

    WriteTextAt(context, font, x, bottomMargin, pageText, fontSize);
}

// Helper function to write the title page
EStatusCode WriteTitlePage(PDFWriter& pdfWriter, PDFUsedFont* font)
{
    PDFPage* titlePage = new PDFPage();
    titlePage->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext* context = pdfWriter.StartPageContentContext(titlePage);
    if (!context)
        return eFailure;

    WriteTextAt(context, font, 200, 700, "User Guide", 24);
    WriteTextAt(context, font, 180, 660, "PDF-Writer Library", 18);
    WriteTextAt(context, font, 220, 400, "Version 1.0", 14);

    EStatusCode status = pdfWriter.EndPageContentContext(context);
    if (status != eSuccess)
        return status;

    return pdfWriter.WritePageAndRelease(titlePage);
}

// Helper function to prepare TOC entries with pre-allocated annotation IDs
void PrepareTOCEntries(PDFWriter& pdfWriter, std::vector<TOCEntry>& outTocEntries)
{
    ObjectsContext& objectsContext = pdfWriter.GetObjectsContext();
    double tocYPosition = 700;
    const double tocLineHeight = 25;

    const char* tocTitles[] = {
        "1. Introduction",
        "   1.1 Getting Started",
        "2. Basic Usage",
        "3. Advanced Features",
        "4. Appendix"
    };
    const int tocPageNumbers[] = {3, 4, 5, 6, 7};
    const int numTocEntries = 5;

    for (int i = 0; i < numTocEntries; ++i)
    {
        TOCEntry entry;
        entry.title = tocTitles[i];
        entry.pageNumber = tocPageNumbers[i];
        entry.annotationID = objectsContext.GetInDirectObjectsRegistry().AllocateNewObjectID();
        entry.pageID = 0;
        entry.yPosition = tocYPosition;
        entry.linkRect = PDFRectangle(65, tocYPosition - 5, 530, tocYPosition + 15);
        outTocEntries.push_back(entry);
        tocYPosition -= tocLineHeight;
    }
}

// Helper function to write the table of contents page
EStatusCode WriteTOCPage(PDFWriter& pdfWriter, PDFUsedFont* font, std::vector<TOCEntry>& tocEntries)
{
    PDFPage* tocPage = new PDFPage();
    tocPage->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext* context = pdfWriter.StartPageContentContext(tocPage);
    if (!context)
        return eFailure;

    WriteTextAt(context, font, 50, 750, "Table of Contents", 20);

    const double leftMargin = 70;
    const double rightMargin = 530;
    const double fontSize = 12;
    const double totalWidth = rightMargin - leftMargin;
    double dotAdvance = font->CalculateTextAdvance(".", fontSize);

    for (size_t i = 0; i < tocEntries.size(); ++i)
    {
        const TOCEntry& entry = tocEntries[i];
        bool isSubsection = (entry.title.length() > 0 && entry.title[0] == ' ');
        double indent = isSubsection ? 15.0 : 0.0;

        char pageNumBuffer[32];
        snprintf(pageNumBuffer, sizeof(pageNumBuffer), "%d", entry.pageNumber);
        std::string pageNumStr(pageNumBuffer);

        double titleWidth = font->CalculateTextAdvance(entry.title, fontSize);
        double pageNumWidth = font->CalculateTextAdvance(pageNumStr, fontSize);
        double dotsWidth = totalWidth - indent - titleWidth - pageNumWidth;
        int numDots = static_cast<int>(dotsWidth / dotAdvance) - 2;

        double titleX = leftMargin + indent;
        double dotsX = titleX + titleWidth + dotAdvance;
        double pageNumX = rightMargin - pageNumWidth;

        WriteTextAt(context, font, titleX, entry.yPosition, entry.title, fontSize);

        if (numDots > 0)
        {
            std::string dots(numDots, '.');
            AbstractContentContext::TextOptions dotOptions(font, fontSize,
                                                           AbstractContentContext::eGray, 178);
            context->WriteText(dotsX, entry.yPosition, dots, dotOptions);
        }

        WriteTextAt(context, font, pageNumX, entry.yPosition, pageNumStr, fontSize);
    }

    EStatusCode status = pdfWriter.EndPageContentContext(context);
    if (status != eSuccess)
        return status;

    DocumentContext& documentContext = pdfWriter.GetDocumentContext();
    for (size_t i = 0; i < tocEntries.size(); ++i)
    {
        documentContext.RegisterAnnotationReferenceForNextPageWrite(tocEntries[i].annotationID);
    }

    return pdfWriter.WritePageAndRelease(tocPage);
}

// Helper function to write a content page with title and multiple text lines
EStatusCodeAndObjectIDType WriteContentPage(
    PDFWriter& pdfWriter,
    PDFUsedFont* font,
    const std::string& title,
    double titleFontSize,
    const char* textLines[],
    int numLines,
    int pageNumber)
{
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext* context = pdfWriter.StartPageContentContext(page);
    if (!context)
        return EStatusCodeAndObjectIDType(eFailure, 0);

    WriteTextAt(context, font, 50, 750, title, titleFontSize);

    double yPosition = 700;
    const double lineSpacing = 30;
    for (int i = 0; i < numLines; ++i)
    {
        WriteTextAt(context, font, 50, yPosition, textLines[i], 12);
        yPosition -= lineSpacing;
    }

    AddPageNumber(context, font, pageNumber);

    EStatusCode status = pdfWriter.EndPageContentContext(context);
    if (status != eSuccess)
        return EStatusCodeAndObjectIDType(eFailure, 0);

    return pdfWriter.WritePageReleaseAndReturnPageID(page);
}

// Helper function to write content page for Introduction section
EStatusCodeAndObjectIDType WriteIntroductionPage(PDFWriter& pdfWriter, PDFUsedFont* font)
{
    const char* lines[] = {
        "Welcome to the PDF-Writer library documentation.",
        "This library provides comprehensive PDF creation capabilities",
        "for C++ developers."
    };
    return WriteContentPage(pdfWriter, font, "1. Introduction", 18, lines, 3, 3);
}

// Helper function to write content page for Getting Started section
EStatusCodeAndObjectIDType WriteGettingStartedPage(PDFWriter& pdfWriter, PDFUsedFont* font)
{
    const char* lines[] = {
        "To get started with PDF-Writer:",
        "1. Include the PDFWriter header",
        "2. Create a PDFWriter instance",
        "3. Start creating your PDF document"
    };
    return WriteContentPage(pdfWriter, font, "1.1 Getting Started", 16, lines, 4, 4);
}

// Helper function to write content page for Basic Usage section
EStatusCodeAndObjectIDType WriteBasicUsagePage(PDFWriter& pdfWriter, PDFUsedFont* font)
{
    const char* lines[] = {
        "Creating pages and adding content is straightforward:",
        "Use PDFPage to create new pages",
        "Use PageContentContext to draw content"
    };
    return WriteContentPage(pdfWriter, font, "2. Basic Usage", 18, lines, 3, 5);
}

// Helper function to write content page for Advanced Features section
EStatusCodeAndObjectIDType WriteAdvancedFeaturesPage(PDFWriter& pdfWriter, PDFUsedFont* font)
{
    const char* lines[] = {
        "PDF-Writer supports many advanced features:",
        "- Forms and annotations",
        "- Image embedding",
        "- Encryption and security",
        "- PDF modification"
    };
    return WriteContentPage(pdfWriter, font, "3. Advanced Features", 18, lines, 5, 6);
}

// Helper function to write content page for Appendix section
EStatusCodeAndObjectIDType WriteAppendixPage(PDFWriter& pdfWriter, PDFUsedFont* font)
{
    const char* lines[] = {
        "Additional resources and references:",
        "- API Reference",
        "- Example code",
        "- License information"
    };
    return WriteContentPage(pdfWriter, font, "4. Appendix", 18, lines, 4, 7);
}

// Helper function to write annotation bodies with page references
void WriteAnnotationBodies(PDFWriter& pdfWriter, const std::vector<TOCEntry>& tocEntries)
{
    for (size_t i = 0; i < tocEntries.size(); ++i)
    {
        const TOCEntry& entry = tocEntries[i];
        WriteInternalPageLinkBody(pdfWriter, entry.annotationID, entry.linkRect, entry.pageID);
    }
}

int TableOfContentsTest(int argc, char* argv[])
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    do
    {
        status = pdfWriter.StartPDF(
            BuildRelativeOutputPath(argv, "TableOfContentsTest.pdf"),
            ePDFVersion13,
            LogConfiguration(true, true,
                BuildRelativeOutputPath(argv, "TableOfContentsTest.log")));
        if (status != eSuccess)
            break;

        PDFUsedFont* font = pdfWriter.GetFontForFile(
            BuildRelativeInputPath(argv, "fonts/arial.ttf"));
        if (!font)
        {
            status = eFailure;
            break;
        }

        status = WriteTitlePage(pdfWriter, font);
        if (status != eSuccess)
            break;

        std::vector<TOCEntry> tocEntries;
        PrepareTOCEntries(pdfWriter, tocEntries);

        status = WriteTOCPage(pdfWriter, font, tocEntries);
        if (status != eSuccess)
            break;

        int entryIndex = 0;

        EStatusCodeAndObjectIDType result = WriteIntroductionPage(pdfWriter, font);
        if (result.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        tocEntries[entryIndex++].pageID = result.second;

        result = WriteGettingStartedPage(pdfWriter, font);
        if (result.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        tocEntries[entryIndex++].pageID = result.second;

        result = WriteBasicUsagePage(pdfWriter, font);
        if (result.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        tocEntries[entryIndex++].pageID = result.second;

        result = WriteAdvancedFeaturesPage(pdfWriter, font);
        if (result.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        tocEntries[entryIndex++].pageID = result.second;

        result = WriteAppendixPage(pdfWriter, font);
        if (result.first != eSuccess)
        {
            status = eFailure;
            break;
        }
        tocEntries[entryIndex++].pageID = result.second;

        WriteAnnotationBodies(pdfWriter, tocEntries);

        status = pdfWriter.EndPDF();
        if (status != eSuccess)
            break;

    } while (false);

    return status == eSuccess ? 0 : 1;
}
