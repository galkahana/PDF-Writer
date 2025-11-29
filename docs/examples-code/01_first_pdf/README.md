# First PDF Example

The simplest possible PDF creation example - creates a blank A4 page.

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Running

```bash
./first_pdf
```

This will create `output.pdf` in the current directory.

## What You'll Learn

- How to create a PDFWriter instance
- How to start a PDF document
- How to create and configure a page
- How to write a page to the PDF
- How to finalize a PDF properly
- Basic error handling pattern

## See Also

- [Documentation: Your First PDF](../../getting-started/first-pdf.md)
- [Documentation: Core Concepts](../../getting-started/core-concepts.md)
