# PDF-to-PDF Operations

Learn how to copy, merge, and reuse content from existing PDFs - a key differentiator of PDF-Writer.

## Overview

PDF-Writer excels at **PDF-to-PDF operations** - copying pages and content from existing PDFs into new documents. This enables powerful workflows:

- **Merge multiple PDFs** into one document
- **Reorder pages** or extract specific pages
- **Reuse pages** as templates across multiple documents
- **Combine PDFs with new content** - text, images, graphics

## Quick Start

Merge multiple PDFs in three lines:

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("merged.pdf", ePDFVersion13);

// Append all pages from each source
pdfWriter.AppendPDFPagesFromPDF("file1.pdf", PDFPageRange());
pdfWriter.AppendPDFPagesFromPDF("file2.pdf", PDFPageRange());
pdfWriter.AppendPDFPagesFromPDF("file3.pdf", PDFPageRange());

pdfWriter.EndPDF();
```

## Two Approaches

PDF-Writer provides two levels of API for PDF-to-PDF operations:

### 1. High-Level: Direct on PDFWriter

Simple, convenient - best for straightforward operations:

```cpp
// Append pages directly
pdfWriter.AppendPDFPagesFromPDF("source.pdf", PDFPageRange());

// Merge pages into existing page
pdfWriter.MergePDFPagesToPage(targetPage, "source.pdf", range);
```

### 2. Low-Level: PDFDocumentCopyingContext

More control - best for complex operations:

```cpp
// Create copying context
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("source.pdf");

// Fine-grained control
ctx->AppendPDFPageFromPDF(2);  // Specific page
ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);  // Reusable template

delete ctx;
```

**Use high-level** for simple merging. **Use low-level** when you need:
- Reusable templates (Form XObjects)
- Custom page order
- Multiple operations on same source
- Advanced resource control

## Appending Pages

### Append All Pages

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

// Append all pages from source
EStatusCodeAndObjectIDTypeList result = pdfWriter.AppendPDFPagesFromPDF(
    "source.pdf",
    PDFPageRange()  // Empty range = all pages
);

if (result.first == eSuccess) {
    ObjectIDTypeList pageIDs = result.second;
    std::cout << "Appended " << pageIDs.size() << " pages" << std::endl;
}

pdfWriter.EndPDF();
```

### Append Specific Pages

```cpp
// Append individual pages by index (0-based)
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("source.pdf");

ctx->AppendPDFPageFromPDF(0);   // First page
ctx->AppendPDFPageFromPDF(3);   // Fourth page
ctx->AppendPDFPageFromPDF(10);  // Eleventh page

delete ctx;
```

### Append Page Ranges

```cpp
// Define specific ranges
PDFPageRange range;
range.mType = PDFPageRange::eRangeTypeSpecific;

// Pages 0-2 (first three)
range.mSpecificRanges.push_back(ULongAndULong(0, 2));

// Pages 5-7
range.mSpecificRanges.push_back(ULongAndULong(5, 7));

EStatusCodeAndObjectIDTypeList result =
    pdfWriter.AppendPDFPagesFromPDF("source.pdf", range);
```

## Merging PDFs

### Merge Multiple Files

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("merged.pdf", ePDFVersion13);

// Append from multiple sources in sequence
pdfWriter.AppendPDFPagesFromPDF("chapter1.pdf", PDFPageRange());
pdfWriter.AppendPDFPagesFromPDF("chapter2.pdf", PDFPageRange());
pdfWriter.AppendPDFPagesFromPDF("chapter3.pdf", PDFPageRange());

pdfWriter.EndPDF();
```

### Interleave Pages from Multiple Sources

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("interleaved.pdf", ePDFVersion13);

PDFDocumentCopyingContext* ctxA = pdfWriter.CreatePDFCopyingContext("fileA.pdf");
PDFDocumentCopyingContext* ctxB = pdfWriter.CreatePDFCopyingContext("fileB.pdf");

// Alternate between sources
ctxA->AppendPDFPageFromPDF(0);  // A page 1
ctxB->AppendPDFPageFromPDF(0);  // B page 1
ctxA->AppendPDFPageFromPDF(1);  // A page 2
ctxB->AppendPDFPageFromPDF(1);  // B page 2

delete ctxA;
delete ctxB;

pdfWriter.EndPDF();
```

## Reordering Pages

### Reverse Page Order

```cpp
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("input.pdf");

unsigned long pageCount = ctx->GetSourceDocumentParser()->GetPagesCount();

// Append in reverse order
for (unsigned long i = 0; i < pageCount; ++i) {
    ctx->AppendPDFPageFromPDF(pageCount - 1 - i);
}

delete ctx;
```

### Custom Page Order

```cpp
// Extract pages in custom sequence
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("source.pdf");

// Reorder: page 6, then 3, then 1, then 9
ctx->AppendPDFPageFromPDF(5);  // 6th page
ctx->AppendPDFPageFromPDF(2);  // 3rd page
ctx->AppendPDFPageFromPDF(0);  // 1st page
ctx->AppendPDFPageFromPDF(8);  // 9th page

delete ctx;
```

## Reusable Templates (Form XObjects)

Form XObjects let you use the same page content multiple times efficiently.

### Create Template from Page

```cpp
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("template.pdf");

// Convert first page to reusable form XObject
EStatusCodeAndObjectIDType result =
    ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);

if (result.first != eSuccess) {
    std::cout << "Failed to create form" << std::endl;
    delete ctx;
    return 1;
}

ObjectIDType templateID = result.second;

// Use template on multiple pages
for (int i = 0; i < 5; i++) {
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext* pageCtx = pdfWriter.StartPageContentContext(page);

    // Place the template
    std::string formName = page->GetResourcesDictionary().AddFormXObjectMapping(templateID);
    pageCtx->Do(formName);

    pdfWriter.EndPageContentContext(pageCtx);
    pdfWriter.WritePageAndRelease(page);
}

delete ctx;
```

### Template with Transformation

```cpp
// Create template with scaling
double scaleMatrix[6] = {0.5, 0, 0, 0.5, 0, 0};  // 50% scale

EStatusCodeAndObjectIDType result = ctx->CreateFormXObjectFromPDFPage(
    0,                       // Page index
    ePDFPageBoxMediaBox,     // Use full page
    scaleMatrix              // Transformation matrix
);

ObjectIDType scaledTemplateID = result.second;
```

### Multiple Templates on One Page

```cpp
// Create two different templates
EStatusCodeAndObjectIDType result1 = ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);
EStatusCodeAndObjectIDType result2 = ctx->CreateFormXObjectFromPDFPage(1, ePDFPageBoxMediaBox);

ObjectIDType template1 = result1.second;
ObjectIDType template2 = result2.second;

// Use both on one page
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* pageCtx = pdfWriter.StartPageContentContext(page);

std::string form1Name = page->GetResourcesDictionary().AddFormXObjectMapping(template1);
std::string form2Name = page->GetResourcesDictionary().AddFormXObjectMapping(template2);

// Place first template (top half)
pageCtx->q();
pageCtx->cm(0.5, 0, 0, 0.5, 0, 421);
pageCtx->Do(form1Name);
pageCtx->Q();

// Place second template (bottom half)
pageCtx->q();
pageCtx->cm(0.5, 0, 0, 0.5, 0, 0);
pageCtx->Do(form2Name);
pageCtx->Q();

pdfWriter.EndPageContentContext(pageCtx);
pdfWriter.WritePageAndRelease(page);
```

## Merging Content onto Pages

Combine existing PDF content with new content on the same page.

### Merge Page with New Content

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("combined.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Add new text
PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");
AbstractContentContext::TextOptions textOpts(font, 24, AbstractContentContext::eGray, 0);
ctx->WriteText(100, 800, "New Header Text", textOpts);

// Merge existing PDF page (scaled to fit below header)
ctx->q();
ctx->cm(1, 0, 0, 0.9, 0, 0);  // Scale vertically to leave room for header

PDFPageRange singlePage;
singlePage.mType = PDFPageRange::eRangeTypeSpecific;
singlePage.mSpecificRanges.push_back(ULongAndULong(0, 0));  // First page only

pdfWriter.MergePDFPagesToPage(page, "content.pdf", singlePage);
ctx->Q();

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

### Merge Using Copying Context

```cpp
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("source.pdf");

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* pageCtx = pdfWriter.StartPageContentContext(page);

// Scale and position merged content
pageCtx->q();
pageCtx->cm(0.5, 0, 0, 0.5, 0, 421);  // Top half

// Merge directly into page
EStatusCode status = ctx->MergePDFPageToPage(page, 0);  // Page index 0

pageCtx->Q();

pdfWriter.EndPageContentContext(pageCtx);
pdfWriter.WritePageAndRelease(page);

delete ctx;
```

## Page Boxes

When creating Form XObjects, choose which box defines the page bounds:

```cpp
enum EPDFPageBox {
    ePDFPageBoxMediaBox,   // Full page size (most common)
    ePDFPageBoxCropBox,    // Visible area when displayed
    ePDFPageBoxBleedBox,   // Printing bleed area
    ePDFPageBoxTrimBox,    // Final trimmed size
    ePDFPageBoxArtBox      // Meaningful content area
};
```

**MediaBox** is typically what you want - it's the full page size.

### Using Different Boxes

```cpp
// Use MediaBox (default, full page)
ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);

// Use CropBox (what PDF viewer displays)
ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxCropBox);

// Use TrimBox (final print size)
ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxTrimBox);
```

### Custom Crop Rectangle

```cpp
// Extract specific region of page
PDFRectangle customCrop(50, 50, 300, 400);  // Define region

EStatusCodeAndObjectIDType result = ctx->CreateFormXObjectFromPDFPage(
    0,
    customCrop  // Instead of EPDFPageBox constant
);
```

## Complete Examples

### Example 1: Simple PDF Merger

```cpp
#include "PDFWriter.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        status = pdfWriter.StartPDF("merged.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        // Merge three files
        EStatusCodeAndObjectIDTypeList result1 =
            pdfWriter.AppendPDFPagesFromPDF("file1.pdf", PDFPageRange());
        if (result1.first != eSuccess) {
            status = result1.first;
            break;
        }

        EStatusCodeAndObjectIDTypeList result2 =
            pdfWriter.AppendPDFPagesFromPDF("file2.pdf", PDFPageRange());
        if (result2.first != eSuccess) {
            status = result2.first;
            break;
        }

        EStatusCodeAndObjectIDTypeList result3 =
            pdfWriter.AppendPDFPagesFromPDF("file3.pdf", PDFPageRange());
        if (result3.first != eSuccess) {
            status = result3.first;
            break;
        }

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Successfully merged PDFs!" << std::endl;
        std::cout << "  File 1: " << result1.second.size() << " pages" << std::endl;
        std::cout << "  File 2: " << result2.second.size() << " pages" << std::endl;
        std::cout << "  File 3: " << result3.second.size() << " pages" << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
```

### Example 2: Page Reordering

```cpp
#include "PDFWriter.h"
#include "PDFDocumentCopyingContext.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext* ctx = NULL;

    do {
        status = pdfWriter.StartPDF("reordered.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        ctx = pdfWriter.CreatePDFCopyingContext("input.pdf");
        if (!ctx) {
            status = eFailure;
            break;
        }

        unsigned long pageCount = ctx->GetSourceDocumentParser()->GetPagesCount();
        std::cout << "Reversing " << pageCount << " pages..." << std::endl;

        // Append pages in reverse order
        for (unsigned long i = 0; i < pageCount; ++i) {
            unsigned long pageIndex = pageCount - 1 - i;

            EStatusCodeAndObjectIDType result = ctx->AppendPDFPageFromPDF(pageIndex);
            if (result.first != eSuccess) {
                std::cout << "Failed to append page " << pageIndex << std::endl;
                status = result.first;
                break;
            }
        }

        if (status != eSuccess) break;

        delete ctx;
        ctx = NULL;

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Successfully reordered PDF!" << std::endl;

    } while(false);

    delete ctx;
    return (status == eSuccess) ? 0 : 1;
}
```

### Example 3: Reusable Template

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFDocumentCopyingContext.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext* ctx = NULL;

    do {
        status = pdfWriter.StartPDF("templated.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        // Create copying context for template
        ctx = pdfWriter.CreatePDFCopyingContext("template.pdf");
        if (!ctx) {
            status = eFailure;
            break;
        }

        // Create reusable form from first page
        EStatusCodeAndObjectIDType result =
            ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);

        if (result.first != eSuccess) {
            status = result.first;
            break;
        }

        ObjectIDType templateID = result.second;
        std::cout << "Created template with ID: " << templateID << std::endl;

        // Create 10 pages using the same template
        for (int i = 0; i < 10; ++i) {
            PDFPage* page = new PDFPage();
            page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

            PageContentContext* pageCtx = pdfWriter.StartPageContentContext(page);

            // Place template
            std::string formName = page->GetResourcesDictionary().AddFormXObjectMapping(templateID);
            pageCtx->Do(formName);

            // Add page number
            PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");
            AbstractContentContext::TextOptions textOpts(font, 12,
                AbstractContentContext::eGray, 0);

            char pageNum[32];
            sprintf(pageNum, "Page %d", i + 1);
            pageCtx->WriteText(50, 50, pageNum, textOpts);

            pdfWriter.EndPageContentContext(pageCtx);
            pdfWriter.WritePageAndRelease(page);
        }

        delete ctx;
        ctx = NULL;

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Successfully created 10 pages from template!" << std::endl;

    } while(false);

    delete ctx;
    return (status == eSuccess) ? 0 : 1;
}
```

### Example 4: Merge with New Content

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "AbstractContentContext.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        status = pdfWriter.StartPDF("combined.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

        // Add header text
        PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");
        AbstractContentContext::TextOptions titleOpts(font, 24,
            AbstractContentContext::eGray, 0);
        ctx->WriteText(100, 800, "Monthly Report - January 2025", titleOpts);

        // Draw separator line
        AbstractContentContext::GraphicOptions lineOpts(
            AbstractContentContext::eStroke,
            AbstractContentContext::eGray, 0, 2);
        ctx->DrawPath(50, 780, 545, 780, lineOpts);

        // Merge existing PDF content below header
        ctx->q();
        ctx->cm(1, 0, 0, 0.9, 0, 0);  // Scale to leave room for header

        PDFPageRange firstPage;
        firstPage.mType = PDFPageRange::eRangeTypeSpecific;
        firstPage.mSpecificRanges.push_back(ULongAndULong(0, 0));

        EStatusCodeAndObjectIDTypeList result =
            pdfWriter.MergePDFPagesToPage(page, "report_data.pdf", firstPage);

        if (result.first != eSuccess) {
            status = result.first;
            ctx->Q();
            break;
        }

        ctx->Q();

        pdfWriter.EndPageContentContext(ctx);
        pdfWriter.WritePageAndRelease(page);

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Successfully combined new content with existing PDF!" << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
```

## Common Issues

### Source PDF Not Found

```cpp
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("missing.pdf");
if (!ctx) {
    std::cout << "Failed to open source PDF" << std::endl;
    // Handle error - file not found or unreadable
}
```

### Invalid Page Index

```cpp
// Get page count first
unsigned long pageCount = ctx->GetSourceDocumentParser()->GetPagesCount();

// Validate index before appending
if (pageIndex >= pageCount) {
    std::cout << "Page index " << pageIndex << " out of range" << std::endl;
    // Handle error
} else {
    ctx->AppendPDFPageFromPDF(pageIndex);
}
```

### Protected/Encrypted PDFs

```cpp
// Attempting to append from protected PDF without password fails
EStatusCodeAndObjectIDTypeList result =
    pdfWriter.AppendPDFPagesFromPDF("protected.pdf", PDFPageRange());

if (result.first != eSuccess) {
    std::cout << "Cannot append from protected PDF without password" << std::endl;
    // Provide password using PDFParsingOptions if you have it
}
```

### Context Deleted Too Early

```cpp
// WRONG - Context deleted before form is finalized
PDFFormXObject* form = pdfWriter.StartFormXObject(PDFRectangle(0, 0, 300, 400));
ctx->MergePDFPageToFormXObject(form, 0);
delete ctx;  // TOO EARLY - resources not yet copied!
pdfWriter.EndFormXObjectAndRelease(form);

// CORRECT - Keep context alive until form is complete
PDFFormXObject* form = pdfWriter.StartFormXObject(PDFRectangle(0, 0, 300, 400));
ctx->MergePDFPageToFormXObject(form, 0);
pdfWriter.EndFormXObjectAndRelease(form);
delete ctx;  // Now safe to delete
```

## Advanced: Copying Annotations

To preserve annotations (links, comments) when copying pages, you need additional steps. See the [Low-Level APIs](low-level-apis.md) guide for annotation copying patterns.

Basic pattern:

```cpp
// Parse source page dictionary
RefCountPtr<PDFDictionary> pageDictionary =
    ctx->GetSourceDocumentParser()->ParsePage(pageIndex);

// Check for annotations
if (pageDictionary->Exists("Annots")) {
    RefCountPtr<PDFObject> annotsObject = pageDictionary->QueryDirectObject("Annots");

    // Deep copy annotations and dependencies
    EStatusCodeAndObjectIDTypeList result =
        ctx->CopyDirectObjectWithDeepCopy(annotsObject.GetPtr());

    // Copy dependencies
    ctx->CopyNewObjectsForDirectObject(result.second);

    // Attach to page being appended
    // (requires annotation writer setup - see Low-Level APIs guide)
}

ctx->AppendPDFPageFromPDF(pageIndex);
```

## Tips and Best Practices

1. **Use high-level for simple merges** - `AppendPDFPagesFromPDF()` is simplest
2. **Create context for complex operations** - Better control over multiple operations
3. **Always delete contexts** - `delete ctx;` calls cleanup automatically
4. **Check page count** - Validate page indices before copying
5. **Reuse templates efficiently** - Form XObjects save space when reused multiple times
6. **Consider transformations** - Scale/position merged content as needed
7. **Handle errors** - Check `EStatusCode` return values
8. **Keep contexts alive** - Don't delete until all dependent operations complete

## Next Steps

- **[Modification](modification.md)** - Edit existing PDFs incrementally
- **[Parsing](parsing.md)** - Read and extract content from PDFs
- **[Low-Level APIs](low-level-apis.md)** - Advanced copying with annotations

## Compilable Example

A complete, compilable example is available at:

- **[examples-code/05_pdf_to_pdf/](../examples-code/05_pdf_to_pdf/)** - Shows merging, reordering, and templates

## See Also

- Test file: PDFWriterTesting/PDFCopyingContextTest.cpp:38
- Test file: PDFWriterTesting/MergePDFPages.cpp:152
- Test file: PDFWriterTesting/MergeToPDFForm.cpp:61
- Test file: PDFWriterTesting/AppendPagesTest.cpp:25
