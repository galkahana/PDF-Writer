# Core Concepts

Understanding these core concepts will help you use PDF-Writer effectively.

## PDF Structure Basics

A PDF file is essentially:
- **Objects**: Dictionaries, arrays, strings, numbers, etc.
- **Pages**: Visual content organized in a page tree
- **Streams**: Compressed content (page graphics, images, fonts)
- **Cross-reference table**: Index of all objects in the file

PDF-Writer handles all of this complexity for you, but understanding it helps when you need more control.

## Library Architecture

PDF-Writer provides multiple levels of abstraction:

### High-Level APIs (Most Users)

**PDFWriter** - Your main entry point
```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("output.pdf", ePDFVersion13);
// ... create content ...
pdfWriter.EndPDF();
```

**High-level content methods**
```cpp
// Simple text placement
AbstractContentContext::TextOptions opts(...);
pageContext->WriteText(100, 700, "Hello World", opts);

// Simple image placement
pageContext->DrawImage(100, 500, "photo.jpg");

// Simple shapes
pageContext->DrawRectangle(50, 50, 200, 100);
pageContext->DrawCircle(300, 300, 50);
```

**When to use**: Most common tasks (90% of use cases)

### Mid-Level APIs (More Control)

**PDF operators via AbstractContentContext**
```cpp
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Use PDF operators directly
ctx->q();                           // Save graphics state
ctx->cm(1, 0, 0, 1, 100, 500);     // Transform
ctx->BT();                          // Begin text
ctx->Tf(font, 14);                  // Set font
ctx->Tj("Hello");                   // Show text
ctx->ET();                          // End text
ctx->Q();                           // Restore graphics state
```

**When to use**: Need precise control over PDF commands, custom layouts

### Low-Level APIs (Maximum Control)

**ObjectsContext** - Direct PDF object writing
```cpp
ObjectsContext& objCtx = pdfWriter.GetObjectsContext();

ObjectIDType myObjID = objCtx.StartNewIndirectObject();
DictionaryContext* dict = objCtx.StartDictionary();
dict->WriteKey("Type");
objCtx.WriteName("CustomThing");
objCtx.EndDictionary(dict);
objCtx.EndIndirectObject();
```

**When to use**: Creating custom PDF elements not supported by high-level APIs (outlines, annotations, ICC profiles, custom structures)

## The PDF-Writer Workflow

### 1. Creation Workflow

For creating PDFs from scratch:

```cpp
PDFWriter pdfWriter;

// Step 1: Start the PDF
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

// Step 2: Create pages and content (repeat as needed)
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
// ... add content ...
pdfWriter.EndPageContentContext(ctx);

pdfWriter.WritePageAndRelease(page);

// Step 3: Finalize
pdfWriter.EndPDF();
```

### 2. Modification Workflow

For editing existing PDFs:

```cpp
PDFWriter pdfWriter;

// Step 1: Open for modification
pdfWriter.ModifyPDF("input.pdf", ePDFVersion13, "output.pdf");

// Step 2: Modify pages
PDFModifiedPage modifiedPage(&pdfWriter, 0);  // Page index 0

AbstractContentContext* ctx = modifiedPage.StartContentContext();
// ... add content ...
modifiedPage.EndContentContext();
modifiedPage.WritePage();

// Step 3: Finalize
pdfWriter.EndPDF();
```

### 3. PDF-to-PDF Workflow

For reusing content from existing PDFs:

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

// Create a copying context
PDFDocumentCopyingContext* copyCtx =
    pdfWriter.CreatePDFCopyingContext("source.pdf");

// Copy pages, create templates, merge content
copyCtx->AppendPDFPageFromPDF(0);  // Copy page 1

pdfWriter.EndPDF();
delete copyCtx;
```

## Coordinate System

PDFs use a **bottom-left origin** coordinate system:

```
(0, 842) ─────────────── (595, 842)
    │                          │
    │                          │
    │        A4 Page          │
    │      595 x 842          │
    │                          │
    │                          │
(0, 0) ───────────────── (595, 0)
```

**Key points**:
- Units are in **points** (1 point = 1/72 inch)
- Origin (0,0) is bottom-left
- Y increases upward
- Standard page sizes:
  - A4: 595 x 842
  - US Letter: 612 x 792
  - Legal: 612 x 1008

**Common mistake**: Placing text at (0, 0) will put it at the bottom of the page, not the top!

```cpp
// Text near top of A4 page
ctx->WriteText(100, 750, "Near top", opts);

// Text near bottom
ctx->WriteText(100, 50, "Near bottom", opts);
```

## Resource Management

### Memory Management Pattern

PDF-Writer uses **manual memory management** (C++98 style):

```cpp
// Create
PDFPage* page = new PDFPage();

// Use
pdfWriter.WritePage(page);

// Clean up
delete page;
```

**Or use the convenience method**:

```cpp
// Automatically deletes after writing
pdfWriter.WritePageAndRelease(page);
```

### Page Lifecycle

**Option 1: Manual cleanup**
```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
pdfWriter.WritePage(page);
delete page;  // Your responsibility
```

**Option 2: Auto cleanup**
```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
pdfWriter.WritePageAndRelease(page);  // PDF-Writer deletes it
// Don't use 'page' after this!
```

### Content Context Lifecycle

```cpp
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
// ... use context ...
pdfWriter.EndPageContentContext(ctx);  // Context is cleaned up automatically
// Don't delete ctx yourself!
```

### Copying Context Lifecycle

```cpp
PDFDocumentCopyingContext* copyCtx =
    pdfWriter.CreatePDFCopyingContext("source.pdf");
// ... use context ...
delete copyCtx;  // Your responsibility to clean up
```

## Error Handling

PDF-Writer uses status codes for error reporting:

```cpp
EStatusCode status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
if (status != eSuccess) {
    // Handle error
}
```

**Standard pattern** (do-while-false):

```cpp
PDFWriter pdfWriter;
EStatusCode status = eSuccess;
PDFPage* page = NULL;

do {
    status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
    if (status != eSuccess) break;

    page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    status = pdfWriter.WritePage(page);
    if (status != eSuccess) break;

    status = pdfWriter.EndPDF();
    if (status != eSuccess) break;

} while(false);

// Cleanup
delete page;

return (status == eSuccess) ? 0 : 1;
```

This pattern ensures cleanup happens regardless of where an error occurs. See [Error Handling Patterns](../guides/error-handling.md) for more details.

## High-Level vs Low-Level: When to Use What

### Use High-Level APIs When:
- Creating standard documents
- Adding text, images, basic shapes
- You want convenience and simplicity
- Examples: Reports, invoices, forms

### Use Mid-Level APIs When:
- Need precise control over graphics state
- Creating complex layouts
- Working with transformations
- Custom text positioning

### Use Low-Level APIs When:
- Creating features not provided by high-level APIs
- Document outlines/bookmarks
- Custom annotations
- ICC color profiles
- Interactive forms
- Any custom PDF structure

**Example - Creating a URL link** (requires low-level):

```cpp
// High-level doesn't provide this, need low-level
ObjectsContext& objCtx = pdfWriter.GetObjectsContext();

ObjectIDType annotID = objCtx.StartNewIndirectObject();
DictionaryContext* dict = objCtx.StartDictionary();

dict->WriteKey("Type");
objCtx.WriteName("Annot");

dict->WriteKey("Subtype");
objCtx.WriteName("Link");

dict->WriteKey("Rect");
objCtx.StartArray();
objCtx.WriteDouble(100);
objCtx.WriteDouble(700);
objCtx.WriteDouble(200);
objCtx.WriteDouble(720);
objCtx.EndArray();

// ... more annotation properties ...

objCtx.EndDictionary(dict);
objCtx.EndIndirectObject();
```

See [Low-Level APIs Guide](../guides/low-level-apis.md) for comprehensive coverage.

## Next Steps

Now that you understand the core concepts:

→ **[Creating PDFs](../guides/creating-pdfs.md)** - Start building real documents
→ **[Text & Fonts](../guides/text-and-fonts.md)** - Add text to your PDFs
→ **[PDF-to-PDF Operations](../guides/pdf-to-pdf.md)** - Reuse existing content

Or dive into specific workflows:

→ **[Modifying PDFs](../guides/modifying-pdfs.md)** - Edit existing files
→ **[Parsing PDFs](../guides/parsing-pdfs.md)** - Read and extract data
