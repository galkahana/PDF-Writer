# Creating PDF Documents

Learn the fundamentals of creating PDF files with PDF-Writer.

## Quick Start

The basic workflow for creating a PDF:

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;

    // 1. Start the PDF
    pdfWriter.StartPDF("output.pdf", ePDFVersion13);

    // 2. Create and write a page
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));  // A4 size
    pdfWriter.WritePageAndRelease(page);

    // 3. Finalize the PDF
    pdfWriter.EndPDF();

    return 0;
}
```

## Starting a PDF

### Basic Initialization

```cpp
PDFWriter pdfWriter;
EStatusCode status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);

if (status != eSuccess) {
    std::cout << "Failed to create PDF" << std::endl;
    return 1;
}
```

### PDF Versions

Choose the appropriate PDF version for your needs:

```cpp
// PDF 1.3 (Acrobat 4.0) - Maximum compatibility
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

// PDF 1.4 (Acrobat 5.0) - Transparency support
pdfWriter.StartPDF("output.pdf", ePDFVersion14);

// PDF 1.5 (Acrobat 6.0) - Compression improvements
pdfWriter.StartPDF("output.pdf", ePDFVersion15);

// PDF 1.6 (Acrobat 7.0)
pdfWriter.StartPDF("output.pdf", ePDFVersion16);

// PDF 1.7 (Acrobat 8.0) - Most common modern version
pdfWriter.StartPDF("output.pdf", ePDFVersion17);

// PDF 2.0 (Latest standard)
pdfWriter.StartPDF("output.pdf", ePDFVersion20);
```

**Recommendation**: Use `ePDFVersion13` for maximum compatibility, or `ePDFVersion17` for modern features.

### Output Options

```cpp
// Write to file path (string)
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

// Write to file path (wide string on Windows)
#ifdef _WIN32
pdfWriter.StartPDF(L"output.pdf", ePDFVersion13);
#endif

// Write to output stream
OutputFile outputFile;
outputFile.OpenFile("output.pdf");
pdfWriter.StartPDF(outputFile.GetOutputStream(), ePDFVersion13);
// Remember to close the file later
outputFile.CloseFile();
```

## Creating Pages

### Page Sizes

Define the page size using `PDFRectangle`:

```cpp
// PDFRectangle(left, bottom, right, top)
// Common page sizes in points (1 point = 1/72 inch)

// A4 (210mm x 297mm)
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

// US Letter (8.5" x 11")
page->SetMediaBox(PDFRectangle(0, 0, 612, 792));

// US Legal (8.5" x 14")
page->SetMediaBox(PDFRectangle(0, 0, 612, 1008));

// Custom size (6" x 9")
page->SetMediaBox(PDFRectangle(0, 0, 432, 648));
```

**Common Sizes Reference**:
- **A4**: 595 x 842 points (210mm x 297mm)
- **Letter**: 612 x 792 points (8.5" x 11")
- **Legal**: 612 x 1008 points (8.5" x 14")
- **Tabloid**: 792 x 1224 points (11" x 17")

### Writing Pages

```cpp
// Create page
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

// Write and release (most common)
pdfWriter.WritePageAndRelease(page);

// Or write and keep reference to page ID
ObjectIDType pageID = pdfWriter.WritePageAndReturnPageID(page);
delete page;  // Clean up manually
```

**Important**: Always create a **new** `PDFPage` object for each page. Do not reuse the same object.

### Multiple Pages

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("multi_page.pdf", ePDFVersion13);

// Write 5 pages
for (int i = 0; i < 5; ++i) {
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    // Add content here if needed

    pdfWriter.WritePageAndRelease(page);
}

pdfWriter.EndPDF();
```

## Page Content

### Adding Content to Pages

Use a `PageContentContext` to draw on pages:

```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

// Start page content context
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Add content using ctx methods
// (see Text & Fonts, Images, Graphics guides for details)

// End page content context
pdfWriter.EndPageContentContext(ctx);

// Write the page
pdfWriter.WritePageAndRelease(page);
```

### Content Workflow

```cpp
// 1. Create page
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

// 2. Get content context
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// 3. Add text
PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");
AbstractContentContext::TextOptions textOpts(font, 14, AbstractContentContext::eGray, 0);
ctx->WriteText(100, 700, "Hello, World!", textOpts);

// 4. Add image
ctx->DrawImage(100, 400, "photo.jpg");

// 5. Add shapes
AbstractContentContext::GraphicOptions rectOpts(AbstractContentContext::eStroke);
ctx->DrawRectangle(100, 100, 200, 150, rectOpts);

// 6. End content context
pdfWriter.EndPageContentContext(ctx);

// 7. Write page
pdfWriter.WritePageAndRelease(page);
```

## Document Properties

### Setting Metadata

```cpp
// Set document information
pdfWriter.GetDocumentContext().GetInfoDictionary()
    .Title = PDFTextString("My Document Title");

pdfWriter.GetDocumentContext().GetInfoDictionary()
    .Author = PDFTextString("Author Name");

pdfWriter.GetDocumentContext().GetInfoDictionary()
    .Subject = PDFTextString("Document Subject");

pdfWriter.GetDocumentContext().GetInfoDictionary()
    .Keywords = PDFTextString("pdf, keywords, example");

pdfWriter.GetDocumentContext().GetInfoDictionary()
    .Creator = PDFTextString("My Application");
```

**Note**: Set metadata after `StartPDF()` but before `EndPDF()`.

### Complete Metadata Example

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("document.pdf", ePDFVersion13);

// Set all metadata fields
InfoDictionary& info = pdfWriter.GetDocumentContext().GetInfoDictionary();
info.Title = PDFTextString("Annual Report 2025");
info.Author = PDFTextString("John Doe");
info.Subject = PDFTextString("Financial Summary");
info.Keywords = PDFTextString("finance, annual, report, 2025");
info.Creator = PDFTextString("ReportGenerator v1.0");

// Create pages...

pdfWriter.EndPDF();
```

## Finalizing the PDF

### Ending the PDF

```cpp
EStatusCode status = pdfWriter.EndPDF();

if (status != eSuccess) {
    std::cout << "Failed to finalize PDF" << std::endl;
    return 1;
}
```

### Complete Workflow with Error Handling

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        // Start PDF
        status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
        if (status != eSuccess) {
            std::cout << "Failed to start PDF" << std::endl;
            break;
        }

        // Set metadata
        pdfWriter.GetDocumentContext().GetInfoDictionary()
            .Title = PDFTextString("My Document");

        // Create and write page
        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

        // Add content...

        pdfWriter.EndPageContentContext(ctx);
        pdfWriter.WritePageAndRelease(page);

        // Finalize PDF
        status = pdfWriter.EndPDF();
        if (status != eSuccess) {
            std::cout << "Failed to finalize PDF" << std::endl;
            break;
        }

        std::cout << "PDF created successfully!" << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
```

## Page Rotation

Set page rotation (in 90-degree increments):

```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

// Rotate page
page->SetRotate(90);   // 90 degrees clockwise
// page->SetRotate(180);  // 180 degrees
// page->SetRotate(270);  // 270 degrees (90 counter-clockwise)

pdfWriter.WritePageAndRelease(page);
```

**Note**: Rotation values must be 0, 90, 180, or 270.

## Common Patterns

### Blank Page

```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
pdfWriter.WritePageAndRelease(page);
```

### Page with Single Text Line

```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");

AbstractContentContext::TextOptions textOpts(font, 24, AbstractContentContext::eGray, 0);
ctx->WriteText(100, 700, "Hello, PDF!", textOpts);

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
```

### Reusing Font Objects

```cpp
// Get font once
PDFUsedFont* font = pdfWriter.GetFontForFile("Helvetica");

// Reuse across multiple pages
for (int i = 0; i < 10; ++i) {
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

    AbstractContentContext::TextOptions textOpts(font, 12, AbstractContentContext::eGray, 0);
    ctx->WriteText(100, 700, "Page content", textOpts);

    pdfWriter.EndPageContentContext(ctx);
    pdfWriter.WritePageAndRelease(page);
}
```

## Common Issues

### Page Not Appearing

**Problem**: Forgot to write the page

```cpp
// WRONG - page created but never written
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
// Missing: pdfWriter.WritePageAndRelease(page);
```

**Solution**: Always call `WritePageAndRelease()` or `WritePageAndReturnPageID()`

### Reusing PDFPage Objects

**Problem**: Using the same `PDFPage` object for multiple pages

```cpp
// WRONG - reusing same page object
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

for (int i = 0; i < 5; ++i) {
    pdfWriter.WritePageAndRelease(page);  // Error after first iteration
}
```

**Solution**: Create a new `PDFPage` for each page

```cpp
// CORRECT - new page object each time
for (int i = 0; i < 5; ++i) {
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
    pdfWriter.WritePageAndRelease(page);
}
```

### Forgot to End PDF

**Problem**: File not properly closed

```cpp
// WRONG - missing EndPDF()
pdfWriter.StartPDF("output.pdf", ePDFVersion13);
// ... create pages ...
// Missing: pdfWriter.EndPDF();
```

**Solution**: Always call `EndPDF()` to finalize the file

### Content Context Not Ended

**Problem**: Forgot to end page content context

```cpp
// WRONG - context not ended
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
ctx->WriteText(100, 700, "Text", textOpts);
// Missing: pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
```

**Solution**: Always end the context before writing the page

## Tips and Best Practices

1. **Always use do-while-false** - Recommended error handling pattern
2. **Check status codes** - Verify `StartPDF()` and `EndPDF()` return values
3. **Create new page objects** - Never reuse `PDFPage` objects
4. **End content contexts** - Call `EndPageContentContext()` before writing pages
5. **Reuse fonts** - Call `GetFontForFile()` once and reuse the font object
6. **Use standard sizes** - A4 (595x842) and Letter (612x792) are most common
7. **Set metadata** - Include title, author, and other info for better organization
8. **Choose appropriate PDF version** - PDF 1.3 for compatibility, 1.7 for modern features

## Next Steps

Now that you understand the basics of creating PDFs, learn how to add content:

→ **[Text & Fonts](text-and-fonts.md)** - Add text with custom fonts
→ **[Images](images.md)** - Embed images in your PDFs
→ **[Links](links.md)** - Make PDFs interactive
→ **[Graphics & Drawing](graphics-and-drawing.md)** - Draw shapes and paths

## See Also

- [First PDF Tutorial](../getting-started/first-pdf.md) - Step-by-step beginner guide
- [Core Concepts](../getting-started/core-concepts.md) - Understanding PDF-Writer architecture
- Test file: PDFWriterTesting/SimpleContentPageTest.cpp:45
