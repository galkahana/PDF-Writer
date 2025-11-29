# Your First PDF

Let's create a simple PDF in just a few minutes. This guide walks through the key concepts, then shows you a complete working example.

## Core Concepts

Creating a PDF with PDF-Writer follows this workflow:

1. **Start the PDF** - Open the file and set the PDF version
2. **Create pages** - Define page size and properties
3. **Add content** - Write text, images, graphics (we'll skip this for now)
4. **Write pages** - Add pages to the document
5. **Finalize** - Close the PDF file

## Key Headers

```cpp
#include "PDFWriter.h"    // Main entry point
#include "PDFPage.h"      // Page definition
#include "PDFRectangle.h" // For page dimensions
```

## Step 1: Start the PDF

```cpp
PDFWriter pdfWriter;
status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
```

**PDF versions available**:
- `ePDFVersion13`, `ePDFVersion14`, `ePDFVersion15`, `ePDFVersion16`, `ePDFVersion17` (recommended)
- `ePDFVersion20` (requires OpenSSL)

**Tip**: Use `ePDFVersion14` or `ePDFVersion17` for maximum compatibility.

## Step 2: Create a Page

```cpp
PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
```

**MediaBox** defines the page size in points (1 point = 1/72 inch):
- **A4**: 595 x 842
- **US Letter**: 612 x 792
- **Legal**: 612 x 1008

## Step 3: Write the Page

```cpp
status = pdfWriter.WritePage(page);
delete page;  // You're responsible for cleanup
```

Or use the convenience method:
```cpp
pdfWriter.WritePageAndRelease(page);  // PDF-Writer deletes it
```

## Step 4: Finalize

```cpp
status = pdfWriter.EndPDF();
```

**Always call `EndPDF()`** to write the PDF footer and close the file properly.

## Error Handling Pattern

The **recommended pattern** for error handling is do-while-false:

```cpp
PDFWriter pdfWriter;
EStatusCode status = eSuccess;
PDFPage* page = NULL;

do {
    status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
    if (status != eSuccess) {
        std::cout << "Failed to start PDF" << std::endl;
        break;  // Exit the loop on error
    }

    // ... more operations with break on error ...

} while(false);

// Cleanup happens here regardless of success/failure
delete page;

return (status == eSuccess) ? 0 : 1;
```

**Why this pattern?**
- Ensures cleanup code always runs
- Easy to add error handling at each step
- Single exit point for the main logic

## Complete Example

Here's the full program putting it all together:

**first_pdf.cpp**:
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
        // Start a new PDF document
        status = pdfWriter.StartPDF("output.pdf", ePDFVersion13);
        if (status != eSuccess) {
            std::cout << "Failed to start PDF" << std::endl;
            break;
        }

        // Create a page (standard A4 size: 595x842 points)
        page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        // Write the page to the PDF
        status = pdfWriter.WritePage(page);
        if (status != eSuccess) {
            std::cout << "Failed to write page" << std::endl;
            break;
        }

        // Finalize the PDF
        status = pdfWriter.EndPDF();
        if (status != eSuccess) {
            std::cout << "Failed to end PDF" << std::endl;
            break;
        }

        std::cout << "Successfully created output.pdf!" << std::endl;

    } while(false);

    // Cleanup
    delete page;

    return (status == eSuccess) ? 0 : 1;
}
```

## Building and Running

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

**Build and run**:
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

## Adding Multiple Pages

Want more than one page? Create and write each page:

```cpp
// Write 5 blank pages
for (int i = 0; i < 5; i++) {
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
    pdfWriter.WritePageAndRelease(page);
}
```

## Next Steps

Now that you can create a basic PDF, let's add some content:

→ **[Adding Text](../guides/text-and-fonts.md)** - Learn how to write text with fonts
→ **[Adding Images](../guides/images.md)** - Embed JPEG, PNG, or TIFF images
→ **[Drawing Graphics](../guides/graphics-and-drawing.md)** - Draw shapes, lines, and paths

Or explore core concepts:

→ **[Core Concepts](core-concepts.md)** - Understand PDF-Writer's architecture

## Compilable Example

A complete, compilable version of this example is available at:

→ **[examples-code/01_first_pdf/](../examples-code/01_first_pdf/)**

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
