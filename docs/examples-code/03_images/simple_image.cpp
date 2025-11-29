/*
 * Simple Image Example
 *
 * Demonstrates:
 * - Loading and placing JPEG images with DrawImage()
 * - Different sizing methods (native, matrix transform, fit to box)
 * - Getting image dimensions
 * - Image positioning
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
        status = pdfWriter.StartPDF("simple_image.pdf", ePDFVersion13);
        if (status != eSuccess) {
            std::cout << "Failed to create PDF" << std::endl;
            break;
        }

        // Create page
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));  // A4

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

        // Note: You'll need to provide a JPEG image file
        // For testing, use any .jpg file and name it "sample.jpg"
        const char* imagePath = "sample.jpg";

        // Check if we can get image dimensions (verifies file exists)
        DoubleAndDoublePair dims = pdfWriter.GetImageDimensions(imagePath);
        if (dims.first == 0 && dims.second == 0) {
            std::cout << "Failed to load image: " << imagePath << std::endl;
            std::cout << "Please provide a JPEG file named 'sample.jpg'" << std::endl;
            status = eFailure;
            break;
        }

        std::cout << "Image dimensions: " << dims.first << " x " << dims.second << std::endl;

        // Example 1: Native size (top of page)
        ctx->DrawImage(50, 650, imagePath);

        // Example 2: Scaled with matrix (50% of original size)
        AbstractContentContext::ImageOptions scaleOpts;
        scaleOpts.transformationMethod = AbstractContentContext::eMatrix;
        scaleOpts.matrix[0] = 0.5;  // Scale X to 50%
        scaleOpts.matrix[3] = 0.5;  // Scale Y to 50%

        ctx->DrawImage(50, 450, imagePath, scaleOpts);

        // Example 3: Fit to bounding box (proportional)
        AbstractContentContext::ImageOptions fitOpts;
        fitOpts.transformationMethod = AbstractContentContext::eFit;
        fitOpts.boundingBoxWidth = 200;
        fitOpts.boundingBoxHeight = 150;
        fitOpts.fitProportional = true;  // Maintain aspect ratio

        ctx->DrawImage(50, 250, imagePath, fitOpts);

        // Example 4: Fit to bounding box (stretch to fill)
        AbstractContentContext::ImageOptions stretchOpts;
        stretchOpts.transformationMethod = AbstractContentContext::eFit;
        stretchOpts.boundingBoxWidth = 200;
        stretchOpts.boundingBoxHeight = 150;
        stretchOpts.fitProportional = false;  // Stretch to fill

        ctx->DrawImage(300, 250, imagePath, stretchOpts);

        // Finalize
        pdfWriter.EndPageContentContext(ctx);
        pdfWriter.WritePageAndRelease(page);

        status = pdfWriter.EndPDF();
        if (status != eSuccess) {
            std::cout << "Failed to finalize PDF" << std::endl;
            break;
        }

        std::cout << "Successfully created simple_image.pdf!" << std::endl;
        std::cout << "The PDF shows the same image 4 different ways:" << std::endl;
        std::cout << "  1. Native size (top left)" << std::endl;
        std::cout << "  2. Scaled 50% (middle left)" << std::endl;
        std::cout << "  3. Fit 200x150 proportional (bottom left)" << std::endl;
        std::cout << "  4. Fit 200x150 stretched (bottom right)" << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
