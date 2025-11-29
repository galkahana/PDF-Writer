# PDF-to-PDF Operations Examples

Examples demonstrating PDF-to-PDF operations in PDF-Writer - a key differentiator feature.

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Running

```bash
./pdf_operations
```

This will create several PDF files demonstrating different operations.

## What You'll Learn

- **Merging PDFs** - Combining multiple PDF files using `AppendPDFPagesFromPDF()`
- **Reordering pages** - Using `PDFDocumentCopyingContext` to control page order
- **Reusable templates** - Creating Form XObjects from pages with `CreateFormXObjectFromPDFPage()`
- **Page counting** - Getting source PDF page count with `GetPagesCount()`
- **Error handling** - Checking status codes throughout operations

## Output Files

The example creates:

1. **file1.pdf** - Sample input (3 pages)
2. **file2.pdf** - Sample input (2 pages)
3. **merged.pdf** - All pages from file1 + file2 (5 pages total)
4. **reordered.pdf** - Pages from file1 in reverse order
5. **templated.pdf** - 3 pages, each using file1's first page as template with overlay text

## Key Concepts

### High-Level Merging
```cpp
// Simple merge - append all pages
pdfWriter.AppendPDFPagesFromPDF("source.pdf", PDFPageRange());
```

### Copying Context for Control
```cpp
// Fine-grained control over which pages and in what order
PDFDocumentCopyingContext* ctx = pdfWriter.CreatePDFCopyingContext("source.pdf");
ctx->AppendPDFPageFromPDF(2);  // Page 3
ctx->AppendPDFPageFromPDF(0);  // Page 1
delete ctx;
```

### Form XObjects for Reuse
```cpp
// Create once, use many times efficiently
EStatusCodeAndObjectIDType result =
    ctx->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);
ObjectIDType templateID = result.second;

// Use on multiple pages
page->GetResourcesDictionary().AddFormXObjectMapping(templateID);
```

## See Also

- [Documentation: PDF-to-PDF Operations](../../guides/pdf-to-pdf.md)
- Test file: PDFWriterTesting/PDFCopyingContextTest.cpp
- Test file: PDFWriterTesting/MergePDFPages.cpp
- Test file: PDFWriterTesting/AppendPagesTest.cpp
