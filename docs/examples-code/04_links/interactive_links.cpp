/*
 * Interactive Links Example
 *
 * Demonstrates:
 * - Creating clickable URL links
 * - Linking text and images
 * - Defining link rectangles
 * - Multiple links on one page
 *
 * Build with CMake (see CMakeLists.txt in this directory)
 */

#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "AbstractContentContext.h"
#include <iostream>

using namespace PDFHummus;

int main(int argc, char* argv[]) {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        // Start PDF
        status = pdfWriter.StartPDF("interactive_links.pdf", ePDFVersion13);
        if (status != eSuccess) {
            std::cout << "Failed to create PDF" << std::endl;
            break;
        }

        // Create page
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));  // A4

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

        // We'll use Helvetica (built-in font, no external file needed)
        PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");

        // Title
        AbstractContentContext::TextOptions titleOpts(font, 18,
            AbstractContentContext::eGray, 0);
        ctx->WriteText(100, 750, "Interactive PDF Demo", titleOpts);

        // Clickable links (blue color for web convention)
        AbstractContentContext::TextOptions linkOpts(font, 12,
            AbstractContentContext::eRGB, 0x0000FF);

        ctx->WriteText(100, 700, "Visit PDF-Writer on GitHub", linkOpts);
        ctx->WriteText(100, 650, "View Documentation", linkOpts);

        // Draw underlines for links (optional but conventional)
        AbstractContentContext::GraphicOptions lineOpts(
            AbstractContentContext::eStroke,
            AbstractContentContext::eRGB, 0x0000FF, 1);

        ctx->DrawPath(100, 698, 280, 698, lineOpts);
        ctx->DrawPath(100, 648, 260, 648, lineOpts);

        // Example 4: Show link area visualization (for demo purposes)
        AbstractContentContext::TextOptions noteOpts(font, 10,
            AbstractContentContext::eGray, 128);
        ctx->WriteText(100, 500, "The blue rectangles above are clickable areas", noteOpts);

        // End content context
        pdfWriter.EndPageContentContext(ctx);

        // Now attach the URLs
        // IMPORTANT: This must be after EndPageContentContext() but before WritePageAndRelease()

        // Link 1: GitHub repository
        pdfWriter.AttachURLLinktoCurrentPage(
            "https://github.com/galkahana/PDF-Writer",
            PDFRectangle(100, 697, 280, 712)
        );

        // Link 2: Documentation website
        pdfWriter.AttachURLLinktoCurrentPage(
            "https://www.pdfhummus.com",
            PDFRectangle(100, 647, 260, 662)
        );

        // Finalize page
        pdfWriter.WritePageAndRelease(page);

        status = pdfWriter.EndPDF();
        if (status != eSuccess) {
            std::cout << "Failed to finalize PDF" << std::endl;
            break;
        }

        std::cout << "Successfully created interactive_links.pdf!" << std::endl;
        std::cout << "Open the PDF and click the blue underlined text to test the links." << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
