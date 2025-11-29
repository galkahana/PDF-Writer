/*
 * Multi-Line Text Example
 *
 * Demonstrates:
 * - Manual text positioning with BT/ET operators
 * - Using Tm and Td for positioning
 * - Multi-line text layout with leading (line spacing)
 * - Text transformation matrix
 *
 * Build with CMake (see CMakeLists.txt in this directory)
 */

#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include <iostream>
#include <vector>

using namespace PDFHummus;

int main(int argc, char* argv[]) {
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF("multiline_text.pdf", ePDFVersion13);
    if (status != eSuccess) {
        std::cout << "Failed to create PDF" << std::endl;
        return 1;
    }

    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

    // Load font
    PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
    if (!font) {
        std::cout << "Failed to load font. Please provide arial.ttf in fonts/ directory" << std::endl;
        pdfWriter.EndPDF();
        delete page;
        return 1;
    }

    // Text settings
    double fontSize = 12;
    double leading = 14;  // Line spacing (slightly larger than font size)
    double x = 50;
    double y = 750;

    // Example 1: Multi-line paragraph using Td (text positioning)
    ctx->BT();                                    // Begin text object
    ctx->Tf(font, 1);                            // Set font (size scaled by matrix)
    ctx->k(0, 0, 0, 1);                          // Black color (CMYK)

    // Transformation matrix: Tm(a, b, c, d, e, f)
    // a,d = scale, b,c = skew, e,f = translation
    ctx->Tm(fontSize, 0, 0, fontSize, x, y);

    ctx->Tj("Multi-Line Text Example");
    ctx->Td(0, -leading);  // Move down by leading
    ctx->Tj("This paragraph demonstrates line spacing.");
    ctx->Td(0, -leading);
    ctx->Tj("Each line is positioned with Td().");
    ctx->Td(0, -leading);
    ctx->Tj("The leading value controls the spacing.");

    ctx->ET();                                    // End text object

    // Example 2: Paragraph with tighter spacing
    y = 650;
    double tightLeading = 11;

    ctx->BT();
    ctx->Tf(font, 1);
    ctx->k(0, 0, 0, 1);
    ctx->Tm(fontSize, 0, 0, fontSize, x, y);

    ctx->Tj("Tight Line Spacing Example");
    ctx->Td(0, -tightLeading);
    ctx->Tj("This paragraph has tighter spacing.");
    ctx->Td(0, -tightLeading);
    ctx->Tj("Leading = 11, Font Size = 12");
    ctx->Td(0, -tightLeading);
    ctx->Tj("Lines are closer together.");

    ctx->ET();

    // Example 3: Using a vector of strings
    y = 550;
    std::vector<std::string> paragraphLines = {
        "Using std::vector for Text Lines",
        "This approach makes it easy to",
        "iterate through multiple lines of text",
        "and position them consistently.",
        "Perfect for generating reports!"
    };

    ctx->BT();
    ctx->Tf(font, 1);
    ctx->k(0, 0, 0, 1);

    for (size_t i = 0; i < paragraphLines.size(); ++i) {
        if (i == 0) {
            // First line: set initial position
            ctx->Tm(fontSize, 0, 0, fontSize, x, y);
        } else {
            // Subsequent lines: move down
            ctx->Td(0, -leading);
        }
        ctx->Tj(paragraphLines[i]);
    }

    ctx->ET();

    // Example 4: Different font sizes in same paragraph
    y = 400;
    double titleSize = 18;
    double bodySize = 11;

    // Title
    ctx->BT();
    ctx->Tf(font, 1);
    ctx->k(0, 0, 0, 1);
    ctx->Tm(titleSize, 0, 0, titleSize, x, y);
    ctx->Tj("Document Title (18pt)");
    ctx->ET();

    // Body text (smaller)
    y -= 25;  // Extra space after title
    ctx->BT();
    ctx->Tf(font, 1);
    ctx->k(0, 0, 0, 1);
    ctx->Tm(bodySize, 0, 0, bodySize, x, y);
    ctx->Tj("Body text in 11pt font.");
    ctx->Td(0, -13);
    ctx->Tj("Second line of body text.");
    ctx->Td(0, -13);
    ctx->Tj("Third line of body text.");
    ctx->ET();

    // Finalize
    pdfWriter.EndPageContentContext(ctx);
    pdfWriter.WritePageAndRelease(page);

    status = pdfWriter.EndPDF();
    if (status != eSuccess) {
        std::cout << "Failed to finalize PDF" << std::endl;
        return 1;
    }

    std::cout << "Successfully created multiline_text.pdf!" << std::endl;
    std::cout << "Open it to see different multi-line text layouts." << std::endl;

    return 0;
}
