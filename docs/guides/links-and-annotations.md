# Links & Annotations

Learn how to make your PDFs interactive with clickable URL links.

## Quick Start

Add a clickable URL link to any area of your page:

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("interactive.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Draw some text
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
AbstractContentContext::TextOptions textOpts(font, 14, AbstractContentContext::eRGB, 0x0000FF);
ctx->WriteText(100, 700, "Click here to visit our website", textOpts);

pdfWriter.EndPageContentContext(ctx);

// Make the text clickable
PDFRectangle linkArea(100, 700, 350, 720);
pdfWriter.AttachURLLinktoCurrentPage("https://example.com", linkArea);

pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

## URL Links

Create clickable links that open web pages.

### Basic URL Link

```cpp
// Define the clickable area (bottom-left to top-right)
PDFRectangle linkArea(100, 700, 300, 720);

// Attach URL to the current page
pdfWriter.AttachURLLinktoCurrentPage("https://github.com/galkahana/PDF-Writer", linkArea);
```

**Important**: Call `AttachURLLinktoCurrentPage()` **after** `EndPageContentContext()` but **before** `WritePageAndRelease()`.

### Link Over Text

```cpp
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

// Draw clickable text
AbstractContentContext::TextOptions textOpts(font, 12, AbstractContentContext::eRGB, 0x0000FF);
ctx->WriteText(100, 700, "Visit Documentation", textOpts);

pdfWriter.EndPageContentContext(ctx);

// Add link over the text
// Calculate width: approximate with character count × font size × 0.5
double textWidth = 19 * 12 * 0.5;  // "Visit Documentation" = 19 chars
PDFRectangle linkRect(100, 700, 100 + textWidth, 700 + 12);
pdfWriter.AttachURLLinktoCurrentPage("https://pdfhummus.com", linkRect);

pdfWriter.WritePageAndRelease(page);
```

**Tip**: For precise text width, use text measurement techniques from the [Text & Fonts guide](text-and-fonts.md#measuring-text).

### Link Over Image

```cpp
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Place an image
DoubleAndDoublePair dims = pdfWriter.GetImageDimensions("logo.jpg");
ctx->DrawImage(100, 500, "logo.jpg");

pdfWriter.EndPageContentContext(ctx);

// Make the entire image clickable
PDFRectangle linkRect(100, 500, 100 + dims.first, 500 + dims.second);
pdfWriter.AttachURLLinktoCurrentPage("https://yourcompany.com", linkRect);

pdfWriter.WritePageAndRelease(page);
```

## Defining Link Areas

The `PDFRectangle` defines where users can click:

```cpp
// PDFRectangle(left, bottom, right, top)
PDFRectangle linkRect(
    100,    // Left X
    700,    // Bottom Y
    300,    // Right X
    720     // Top Y
);
```

**Remember**: PDF uses bottom-left origin coordinates (see [Core Concepts](../getting-started/core-concepts.md#coordinate-system)).

### Multiple Links on One Page

```cpp
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Draw multiple clickable elements
AbstractContentContext::TextOptions textOpts(font, 12, AbstractContentContext::eRGB, 0x0000FF);
ctx->WriteText(100, 700, "Documentation", textOpts);
ctx->WriteText(100, 650, "GitHub Repository", textOpts);
ctx->WriteText(100, 600, "Contact Us", textOpts);

pdfWriter.EndPageContentContext(ctx);

// Add links for each item
pdfWriter.AttachURLLinktoCurrentPage("https://docs.example.com",
    PDFRectangle(100, 700, 250, 715));

pdfWriter.AttachURLLinktoCurrentPage("https://github.com/example/repo",
    PDFRectangle(100, 650, 280, 665));

pdfWriter.AttachURLLinktoCurrentPage("https://example.com/contact",
    PDFRectangle(100, 600, 220, 615));

pdfWriter.WritePageAndRelease(page);
```

## Complete Examples

### Example 1: Interactive Document with Links

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "AbstractContentContext.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        status = pdfWriter.StartPDF("links_demo.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
        PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

        // Title
        AbstractContentContext::TextOptions titleOpts(font, 18,
            AbstractContentContext::eGray, 0);
        ctx->WriteText(100, 750, "Interactive PDF Demo", titleOpts);

        // Clickable links (blue underlined style)
        AbstractContentContext::TextOptions linkOpts(font, 12,
            AbstractContentContext::eRGB, 0x0000FF);

        ctx->WriteText(100, 700, "Visit Our Website", linkOpts);
        ctx->WriteText(100, 650, "View Documentation", linkOpts);
        ctx->WriteText(100, 600, "Contact Support", linkOpts);

        // Draw underlines for links
        AbstractContentContext::GraphicOptions lineOpts(
            AbstractContentContext::eStroke,
            AbstractContentContext::eRGB, 0x0000FF, 1);

        ctx->DrawPath(100, 698, 230, 698, lineOpts);
        ctx->DrawPath(100, 648, 250, 648, lineOpts);
        ctx->DrawPath(100, 598, 225, 598, lineOpts);

        pdfWriter.EndPageContentContext(ctx);

        // Attach URLs
        pdfWriter.AttachURLLinktoCurrentPage("https://example.com",
            PDFRectangle(100, 697, 230, 712));

        pdfWriter.AttachURLLinktoCurrentPage("https://docs.example.com",
            PDFRectangle(100, 647, 250, 662));

        pdfWriter.AttachURLLinktoCurrentPage("https://example.com/contact",
            PDFRectangle(100, 597, 225, 612));

        pdfWriter.WritePageAndRelease(page);

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Created links_demo.pdf successfully!" << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
```

### Example 2: Clickable Image Gallery

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "AbstractContentContext.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        status = pdfWriter.StartPDF("gallery.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

        // Place images in grid
        const char* images[] = {"photo1.jpg", "photo2.jpg", "photo3.jpg"};
        const char* urls[] = {
            "https://example.com/photo1",
            "https://example.com/photo2",
            "https://example.com/photo3"
        };

        double x = 50;
        double y = 600;

        for (int i = 0; i < 3; i++) {
            // Get dimensions and scale to thumbnail
            DoubleAndDoublePair dims = pdfWriter.GetImageDimensions(images[i]);

            AbstractContentContext::ImageOptions opts;
            opts.transformationMethod = AbstractContentContext::eFit;
            opts.boundingBoxWidth = 150;
            opts.boundingBoxHeight = 150;
            opts.fitProportional = true;

            ctx->DrawImage(x, y, images[i], opts);

            x += 170;  // Space between images
        }

        pdfWriter.EndPageContentContext(ctx);

        // Add clickable links over each image
        x = 50;
        for (int i = 0; i < 3; i++) {
            PDFRectangle linkRect(x, y, x + 150, y + 150);
            pdfWriter.AttachURLLinktoCurrentPage(urls[i], linkRect);
            x += 170;
        }

        pdfWriter.WritePageAndRelease(page);

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Created gallery.pdf with clickable images!" << std::endl;

    } while(false);

    return (status == eSuccess) ? 0 : 1;
}
```

## Common Issues

### Link Not Clickable

**Problem**: Link area defined incorrectly

```cpp
// WRONG - Rectangle too small or wrong coordinates
PDFRectangle linkRect(100, 700, 110, 705);  // Too small

// CORRECT - Cover the entire text area
PDFRectangle linkRect(100, 700, 250, 715);
```

### Link Added Before Content

**Problem**: Calling `AttachURLLinktoCurrentPage()` at wrong time

```cpp
// WRONG - Link added before content context ended
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
pdfWriter.AttachURLLinktoCurrentPage("https://example.com", rect);  // Too early!
ctx->WriteText(100, 700, "Click here", textOpts);
pdfWriter.EndPageContentContext(ctx);

// CORRECT - Link added after content, before write
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
ctx->WriteText(100, 700, "Click here", textOpts);
pdfWriter.EndPageContentContext(ctx);
pdfWriter.AttachURLLinktoCurrentPage("https://example.com", rect);  // Right time
pdfWriter.WritePageAndRelease(page);
```

## Other Annotations

URL links are the high-level annotation feature available in PDF-Writer. For other annotation types (comments, highlights, shapes, stamps, etc.), you'll need to use lower-level APIs with `ObjectsContext`.

See the [Low-Level APIs](low-level-apis.md) guide for creating advanced annotations.

## Tips and Best Practices

1. **Visual feedback** - Use blue color and underlines for clickable text
2. **Test link areas** - Verify rectangles cover the intended area
3. **Timing matters** - Always add links after `EndPageContentContext()`
4. **Link validation** - Verify URLs are properly formatted before adding
5. **Rectangle precision** - Use `GetImageDimensions()` for exact image link areas

## Next Steps

- **[PDF-to-PDF Operations](pdf-to-pdf.md)** - Combine pages from multiple PDFs
- **[Modification](modification.md)** - Edit existing PDFs
- **[Low-Level APIs](low-level-apis.md)** - Advanced annotation creation

## Compilable Example

A complete, compilable example is available at:

- **[examples-code/04_links/](../examples-code/04_links/)** - Interactive document with clickable links

## See Also

- [Text & Fonts](text-and-fonts.md#measuring-text) - Text measurement for precise link areas
- [Images](images.md) - Making images clickable
- Test file: PDFWriterTesting/LinksTest.cpp:35
