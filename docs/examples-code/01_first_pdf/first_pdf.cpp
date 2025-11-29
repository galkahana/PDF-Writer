/*
 * First PDF Example
 *
 * This example demonstrates the simplest possible PDF creation:
 * - Create a PDFWriter instance
 * - Start a PDF file
 * - Add a blank page
 * - Finalize the PDF
 *
 * Build with CMake (see CMakeLists.txt in this directory)
 */

#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status;

    // Start a new PDF document
    status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
    if (status != eSuccess) {
        std::cout << "Failed to create PDF" << std::endl;
        return 1;
    }

    // Create a page (standard A4 size: 595x842 points)
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    // Write the page to the PDF
    status = pdfWriter.WritePage(page);
    if (status != eSuccess) {
        std::cout << "Failed to write page" << std::endl;
        delete page;
        return 1;
    }

    delete page;

    // Finalize the PDF
    status = pdfWriter.EndPDF();
    if (status != eSuccess) {
        std::cout << "Failed to finalize PDF" << std::endl;
        return 1;
    }

    std::cout << "Successfully created output.pdf!" << std::endl;
    return 0;
}
