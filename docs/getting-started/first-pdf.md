# Your First PDF

Let's create a simple PDF in just a few minutes. This guide walks through a minimal working example that creates a PDF with empty pages.

## The Simplest Possible PDF

Here's a complete program that creates a valid PDF file:

```cpp
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
```

**Save this as `first_pdf.cpp`**

## Building and Running

Assuming you've set up PDF-Writer with CMake (see [Installation](installation.md)):

**CMakeLists.txt**:
```cmake
cmake_minimum_required(VERSION 3.14)
project(FirstPDF)

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.8.0
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)

add_executable(first_pdf first_pdf.cpp)
target_link_libraries(first_pdf PDFHummus::PDFWriter)
```

**Build**:
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
./first_pdf
```

You should see:
```
Successfully created output.pdf!
```

Open `output.pdf` in any PDF viewer - you'll see a blank A4 page!

## Understanding the Code

Let's break down what's happening:

### 1. Include Headers

```cpp
#include "PDFWriter.h"    // Main entry point
#include "PDFPage.h"      // Page definition
#include "PDFRectangle.h" // For page dimensions
```

These are the core headers you'll use in most PDF-Writer programs.

### 2. Create the PDFWriter

```cpp
PDFWriter pdfWriter;
```

`PDFWriter` is your main interface to the library. It manages the PDF file and coordinates all operations.

### 3. Start the PDF

```cpp
status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
```

- **First parameter**: Output file path
- **Second parameter**: PDF version
  - `ePDFVersion10` (PDF 1.0)
  - `ePDFVersion11` (PDF 1.1)
  - `ePDFVersion12` (PDF 1.2)
  - `ePDFVersion13` (PDF 1.3)
  - `ePDFVersion14` (PDF 1.4)
  - `ePDFVersion15` (PDF 1.5)
  - `ePDFVersion16` (PDF 1.6)
  - `ePDFVersion17` (PDF 1.7)
  - `ePDFVersion20` (PDF 2.0 - requires OpenSSL)

**Tip**: Use `ePDFVersion14` or `ePDFVersion17` for maximum compatibility.

### 4. Create a Page

```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
```

- **MediaBox**: Defines the page size in points (1 point = 1/72 inch)
- **595x842**: Standard A4 size
- **Common sizes**:
  - US Letter: 612x792
  - A4: 595x842
  - Legal: 612x1008

### 5. Write the Page

```cpp
status = pdfWriter.WritePage(page);
delete page;
```

`WritePage()` adds the page to the PDF. After writing, we're responsible for deleting the page object.

### 6. Finalize the PDF

```cpp
status = pdfWriter.EndPDF();
```

This writes the PDF footer, cross-reference table, and closes the file. **Always call `EndPDF()`** to ensure a valid PDF.

## Error Handling Pattern

Notice the pattern used throughout:

```cpp
status = someOperation();
if (status != eSuccess) {
    // Handle error
    return 1;
}
```

This is the standard error handling approach in PDF-Writer. `EStatusCode` can be:
- `eSuccess` - Operation succeeded
- `eFailure` - Operation failed

## Adding Multiple Pages

Want more than one page? Just create and write multiple pages:

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("multi-page.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

// Write 5 pages
for (int i = 0; i < 5; i++) {
    pdfWriter.WritePage(page);
}

delete page;
pdfWriter.EndPDF();
```

**Note**: You can reuse the same `PDFPage` object if all pages have the same properties.

## Next Steps

Now that you can create a basic PDF, let's add some content:

→ **[Adding Text](../guides/text-and-fonts.md)** - Learn how to write text with fonts
→ **[Adding Images](../guides/images.md)** - Embed JPEG, PNG, or TIFF images
→ **[Drawing Graphics](../guides/graphics-and-drawing.md)** - Draw shapes, lines, and paths

Or explore core concepts:

→ **[Core Concepts](core-concepts.md)** - Understand PDF-Writer's architecture

## Complete Example with Error Handling

Here's a production-ready version with proper error handling and cleanup:

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;
    PDFPage* page = NULL;

    do {
        // Start PDF
        status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
        if (status != eSuccess) {
            std::cout << "Failed to start PDF" << std::endl;
            break;
        }

        // Create and write page
        page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        status = pdfWriter.WritePage(page);
        if (status != eSuccess) {
            std::cout << "Failed to write page" << std::endl;
            break;
        }

        // Finalize
        status = pdfWriter.EndPDF();
        if (status != eSuccess) {
            std::cout << "Failed to end PDF" << std::endl;
            break;
        }

        std::cout << "Success!" << std::endl;

    } while(false);

    // Cleanup
    delete page;

    return (status == eSuccess) ? 0 : 1;
}
```

This uses the **do-while-false pattern** commonly used in PDF-Writer for error handling with multiple cleanup steps. See [Error Handling Patterns](../guides/error-handling.md) for more details.

## Compilable Example

A complete, compilable version of this example is available at:

→ **[examples-code/01_first_pdf/](../../examples-code/01_first_pdf/)** (coming soon)

## Troubleshooting

**"File not created"**
- Check write permissions in the output directory
- Ensure you called `EndPDF()` to finalize the file

**"Invalid PDF"**
- Make sure you called `EndPDF()` before program exit
- Check that `StartPDF()` returned `eSuccess`

**"Compiler can't find PDFWriter.h"**
- Verify you linked with `PDFHummus::PDFWriter` in CMake
- See [Installation](installation.md) for setup instructions
