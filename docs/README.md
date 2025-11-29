# PDF-Writer User Guide

Welcome to the PDF-Writer user guide! PDF-Writer is a comprehensive C++ library for creating, parsing, and modifying PDF files.

## What Makes PDF-Writer Different?

- **Complete control**: From high-level convenience methods to low-level PDF object creation
- **PDF-to-PDF operations**: Powerful capabilities for reusing existing PDF content - create templates, merge documents, compose pages
- **Parsing & modification**: Not just creation - read, extract, and modify existing PDFs
- **Cross-platform**: Works on Windows, macOS, Linux, iOS
- **Bundled dependencies**: All libraries included (FreeType, Zlib, LibPng, LibTiff, etc.) - just add to your project and build
- **Minimal external dependencies**: Only OpenSSL is external (optional, for PDF 2.0 encryption only)

## Quick Start

New to PDF-Writer? Start here:

1. **[Installation](getting-started/installation.md)** - Set up PDF-Writer in your project
2. **[Your First PDF](getting-started/first-pdf.md)** - Create a PDF in 5 minutes
3. **[Core Concepts](getting-started/core-concepts.md)** - Understand how PDF-Writer works

## User Guide

### Creating PDFs

- **[Creating PDF Documents](guides/creating-pdfs.md)** - Starting a PDF, adding pages, document properties
- **[Text & Fonts](guides/text-and-fonts.md)** - Font loading, text rendering, Unicode support
- **[Images](guides/images.md)** - Embedding JPEG, PNG, TIFF images
- **[Graphics & Drawing](guides/graphics-and-drawing.md)** - Shapes, paths, colors, line styles

### Working with Existing PDFs

- **[PDF-to-PDF Operations](guides/pdf-to-pdf.md)** ⭐ - Reusing existing PDF content
  - Copying pages between documents
  - Creating reusable templates (Form XObjects)
  - Merging pages (N-up layouts, watermarking)
  - Handling annotations and links

- **[Modifying PDFs](guides/modifying-pdfs.md)** - Editing existing PDF files
  - Adding content to existing pages
  - Changing page properties
  - Page manipulation

- **[Parsing PDFs](guides/parsing-pdfs.md)** - Reading and extracting from PDFs
  - Document structure navigation
  - Extracting text and images
  - Reading form values
  - Handling encrypted PDFs

### Advanced Topics

- **[Low-Level APIs](guides/low-level-apis.md)** - Direct PDF object creation with ObjectsContext
- **[Encryption & Security](guides/encryption-and-security.md)** - Password protection and encryption
- **[Document Outlines](guides/outlines.md)** - Creating bookmarks and navigation
- **[Links](guides/links-and-annotations.md)** - Creating clickable URL links

### Best Practices

- **[Error Handling](guides/error-handling.md)** - Proper cleanup and error management patterns
- **[Memory Management](guides/memory-management.md)** - Resource lifecycle and cleanup
- **[Performance Tips](guides/performance.md)** - Optimizing PDF generation and processing
- **[Troubleshooting](guides/troubleshooting.md)** - Common issues and solutions

## Code Examples

Complete, compilable examples with build instructions:

- **[First PDF](examples-code/01_first_pdf/)** - Minimal working example
- **[Text & Fonts](examples-code/02_text_and_fonts/)** - Font loading and text rendering
- **Images** (coming soon) - Working with JPEG, PNG, TIFF
- **PDF-to-PDF** (coming soon) - Reusing existing content
- **Parsing** (coming soon) - Reading and extracting data
- **Modification** (coming soon) - Editing existing PDFs

## Additional Resources

- **[GitHub Repository](https://github.com/galkahana/PDF-Writer)** - Source code and issues
- **[Wiki](https://github.com/galkahana/PDF-Writer/wiki)** - Legacy documentation (being replaced by this guide)
- **[Blog](https://www.pdfhummus.com)** - Development updates and articles
- **[Test Suite](../PDFWriterTesting/)** - 79 tests demonstrating library features

## Contributing

Found an error or want to improve the docs? See [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines.

---

**Looking for something specific?** Use your browser's search (Ctrl/Cmd+F) or check the [Wiki](https://github.com/galkahana/PDF-Writer/wiki) for additional topics.
