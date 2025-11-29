# Links & Annotations Examples

Examples demonstrating interactive PDF features in PDF-Writer.

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Running

```bash
./interactive_links
```

This will create `interactive_links.pdf` with clickable URL links.

## What You'll Learn

- Creating clickable URL links with `AttachURLLinktoCurrentPage()`
- Defining link click areas with `PDFRectangle`
- Proper timing: adding links after `EndPageContentContext()` but before `WritePageAndRelease()`
- Using built-in fonts (Helvetica) that don't require external files
- Visual conventions for links (blue color, underlines)

## Testing the Links

Open `interactive_links.pdf` in a PDF viewer and click the blue underlined text:
1. **Visit PDF-Writer on GitHub** - Opens the GitHub repository
2. **View Documentation** - Opens the PDF-Writer website

## See Also

- [Documentation: Links & Annotations](../../guides/links-and-annotations.md)
- Test file: PDFWriterTesting/LinksTest.cpp
- Test file: PDFWriterTesting/AppendAndReplaceURLAnnotations.cpp
