/*
 * Simple Text Example
 *
 * Demonstrates:
 * - Loading a TrueType font
 * - Using WriteText() high-level API
 * - Different text colors and sizes
 * - Text positioning
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
    EStatusCode status;

    // Start PDF
    status = pdfWriter.StartPDF("simple_text.pdf", ePDFVersion13);
    if (status != eSuccess) {
        std::cout << "Failed to create PDF" << std::endl;
        return 1;
    }

    // Create page
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));  // A4

    PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

    // Load font
    // Note: You'll need to provide a path to a .ttf font file
    // For testing, you can use system fonts (e.g., on Windows: C:/Windows/Fonts/arial.ttf)
    PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
    if (!font) {
        std::cout << "Failed to load font. Please check the font path." << std::endl;
        std::cout << "You may need to create a 'fonts' directory and add arial.ttf" << std::endl;
        pdfWriter.EndPDF();
        delete page;
        return 1;
    }

    // Example 1: Simple black text
    AbstractContentContext::TextOptions blackText(
        font,
        14,
        AbstractContentContext::eGray,
        0  // Black
    );
    ctx->WriteText(50, 750, "Simple black text (14pt)", blackText);

    // Example 2: Larger red text
    AbstractContentContext::TextOptions redText(
        font,
        24,
        AbstractContentContext::eRGB,
        0xFF0000  // Red
    );
    ctx->WriteText(50, 700, "Large red text (24pt)", redText);

    // Example 3: Blue text
    AbstractContentContext::TextOptions blueText(
        font,
        18,
        AbstractContentContext::eRGB,
        0x0000FF  // Blue
    );
    ctx->WriteText(50, 650, "Blue text (18pt)", blueText);

    // Example 4: Gray text
    AbstractContentContext::TextOptions grayText(
        font,
        12,
        AbstractContentContext::eGray,
        128  // Mid gray (0=black, 255=white)
    );
    ctx->WriteText(50, 600, "Gray text (12pt)", grayText);

    // Example 5: Semi-transparent text
    AbstractContentContext::TextOptions transparentText(
        font,
        16,
        AbstractContentContext::eRGB,
        0x000000,  // Black
        0.5        // 50% opacity
    );
    ctx->WriteText(50, 550, "50% transparent text (16pt)", transparentText);

    // Example 6: Multiple lines at different positions
    AbstractContentContext::TextOptions normalText(font, 11, eGray, 0);
    ctx->WriteText(50, 500, "Line 1: This is a paragraph.", normalText);
    ctx->WriteText(50, 485, "Line 2: Each line is positioned separately.", normalText);
    ctx->WriteText(50, 470, "Line 3: Y coordinate decreases as we go down.", normalText);
    ctx->WriteText(50, 455, "Line 4: Remember: (0,0) is bottom-left!", normalText);

    // Finalize
    pdfWriter.EndPageContentContext(ctx);
    pdfWriter.WritePageAndRelease(page);

    status = pdfWriter.EndPDF();
    if (status != eSuccess) {
        std::cout << "Failed to finalize PDF" << std::endl;
        return 1;
    }

    std::cout << "Successfully created simple_text.pdf!" << std::endl;
    std::cout << "Open it to see different text styles and colors." << std::endl;

    return 0;
}
