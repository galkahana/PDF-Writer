/*
 * PDF-to-PDF Operations Example
 *
 * Demonstrates:
 * - Merging multiple PDFs
 * - Reordering pages
 * - Creating reusable templates from pages
 * - Using PDFDocumentCopyingContext
 *
 * Build with CMake (see CMakeLists.txt in this directory)
 */

#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "AbstractContentContext.h"
#include "PDFDocumentCopyingContext.h"
#include <iostream>

using namespace PDFHummus;

// Create sample source PDFs for demonstration
EStatusCode CreateSamplePDF(const char* filename, const char* text, int pageCount) {
    PDFWriter pdfWriter;
    EStatusCode status = pdfWriter.StartPDF(filename, ePDFVersion13);
    if (status != eSuccess) return status;

    PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");

    for (int i = 0; i < pageCount; ++i) {
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

        // Add some text to identify the page
        AbstractContentContext::TextOptions textOpts(font, 24,
            AbstractContentContext::eGray, 0);

        char buffer[256];
        sprintf(buffer, "%s - Page %d", text, i + 1);
        ctx->WriteText(100, 400, buffer, textOpts);

        pdfWriter.EndPageContentContext(ctx);
        pdfWriter.WritePageAndRelease(page);
    }

    return pdfWriter.EndPDF();
}

// Example 1: Simple PDF merge
EStatusCode MergePDFsExample() {
    std::cout << "\n=== Example 1: Merging PDFs ===" << std::endl;

    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        status = pdfWriter.StartPDF("merged.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        // Append all pages from multiple sources
        std::cout << "Merging file1.pdf..." << std::endl;
        EStatusCodeAndObjectIDTypeList result1 =
            pdfWriter.AppendPDFPagesFromPDF("file1.pdf", PDFPageRange());
        if (result1.first != eSuccess) {
            status = result1.first;
            break;
        }

        std::cout << "Merging file2.pdf..." << std::endl;
        EStatusCodeAndObjectIDTypeList result2 =
            pdfWriter.AppendPDFPagesFromPDF("file2.pdf", PDFPageRange());
        if (result2.first != eSuccess) {
            status = result2.first;
            break;
        }

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Success! Created merged.pdf" << std::endl;
        std::cout << "  - " << result1.second.size() << " pages from file1.pdf" << std::endl;
        std::cout << "  - " << result2.second.size() << " pages from file2.pdf" << std::endl;

    } while(false);

    return status;
}

// Example 2: Reorder pages
EStatusCode ReorderPagesExample() {
    std::cout << "\n=== Example 2: Reordering Pages ===" << std::endl;

    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext* ctx = NULL;

    do {
        status = pdfWriter.StartPDF("reordered.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        ctx = pdfWriter.CreatePDFCopyingContext("file1.pdf");
        if (!ctx) {
            status = eFailure;
            break;
        }

        unsigned long pageCount = ctx->GetSourceDocumentParser()->GetPagesCount();
        std::cout << "Reversing " << pageCount << " pages from file1.pdf..." << std::endl;

        // Append in reverse order
        for (unsigned long i = 0; i < pageCount; ++i) {
            unsigned long pageIndex = pageCount - 1 - i;
            EStatusCodeAndObjectIDType result = ctx->AppendPDFPageFromPDF(pageIndex);

            if (result.first != eSuccess) {
                status = result.first;
                break;
            }
        }

        if (status != eSuccess) break;

        delete ctx;
        ctx = NULL;

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Success! Created reordered.pdf with pages in reverse order" << std::endl;

    } while(false);

    delete ctx;
    return status;
}

// Example 3: Reusable template
EStatusCode TemplateExample() {
    std::cout << "\n=== Example 3: Reusable Template ===" << std::endl;

    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext* ctx = NULL;

    do {
        status = pdfWriter.StartPDF("templated.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        // Create copying context
        ctx = pdfWriter.CreatePDFCopyingContext("file1.pdf");
        if (!ctx) {
            status = eFailure;
            break;
        }

        // Create reusable form XObject from first page
        std::cout << "Creating reusable template from file1.pdf page 1..." << std::endl;
        EStatusCodeAndObjectIDType result =
            ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);

        if (result.first != eSuccess) {
            status = result.first;
            break;
        }

        ObjectIDType templateID = result.second;

        // Create 3 pages using the same template
        PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");

        for (int i = 0; i < 3; ++i) {
            PDFPage* page = new PDFPage();
            page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

            PageContentContext* pageCtx = pdfWriter.StartPageContentContext(page);

            // Place the template
            std::string formName = page->GetResourcesDictionary().AddFormXObjectMapping(templateID);
            pageCtx->Do(formName);

            // Add page-specific text overlay
            AbstractContentContext::TextOptions textOpts(font, 18,
                AbstractContentContext::eRGB, 0xFF0000);  // Red text

            char buffer[64];
            sprintf(buffer, "Copy %d of Template", i + 1);
            pageCtx->WriteText(100, 100, buffer, textOpts);

            pdfWriter.EndPageContentContext(pageCtx);
            pdfWriter.WritePageAndRelease(page);
        }

        delete ctx;
        ctx = NULL;

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Success! Created templated.pdf with 3 pages using one template" << std::endl;

    } while(false);

    delete ctx;
    return status;
}

int main(int argc, char* argv[]) {
    EStatusCode status = eSuccess;

    std::cout << "PDF-to-PDF Operations Demo" << std::endl;
    std::cout << "==========================" << std::endl;

    // Create sample PDFs for demonstration
    std::cout << "\nCreating sample PDFs..." << std::endl;

    status = CreateSamplePDF("file1.pdf", "File 1", 3);
    if (status != eSuccess) {
        std::cout << "Failed to create file1.pdf" << std::endl;
        return 1;
    }
    std::cout << "Created file1.pdf (3 pages)" << std::endl;

    status = CreateSamplePDF("file2.pdf", "File 2", 2);
    if (status != eSuccess) {
        std::cout << "Failed to create file2.pdf" << std::endl;
        return 1;
    }
    std::cout << "Created file2.pdf (2 pages)" << std::endl;

    // Run examples
    status = MergePDFsExample();
    if (status != eSuccess) {
        std::cout << "Merge example failed" << std::endl;
        return 1;
    }

    status = ReorderPagesExample();
    if (status != eSuccess) {
        std::cout << "Reorder example failed" << std::endl;
        return 1;
    }

    status = TemplateExample();
    if (status != eSuccess) {
        std::cout << "Template example failed" << std::endl;
        return 1;
    }

    std::cout << "\n=== All Examples Complete ===" << std::endl;
    std::cout << "Output files created:" << std::endl;
    std::cout << "  - file1.pdf (sample input)" << std::endl;
    std::cout << "  - file2.pdf (sample input)" << std::endl;
    std::cout << "  - merged.pdf (file1 + file2)" << std::endl;
    std::cout << "  - reordered.pdf (file1 pages reversed)" << std::endl;
    std::cout << "  - templated.pdf (3 copies of file1 page 1)" << std::endl;

    return 0;
}
