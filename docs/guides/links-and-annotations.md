# Links & Annotations

Learn how to make your PDFs interactive with clickable links and annotations.

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

Create clickable links that open web pages or email addresses.

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

### Email Links

```cpp
// Create mailto link
pdfWriter.AttachURLLinktoCurrentPage("mailto:support@example.com", linkRect);

// With subject and body (URL-encoded)
pdfWriter.AttachURLLinktoCurrentPage(
    "mailto:support@example.com?subject=Feedback&body=Hello",
    linkRect
);
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

pdfWriter.AttachURLLinktoCurrentPage("mailto:contact@example.com",
    PDFRectangle(100, 600, 220, 615));

pdfWriter.WritePageAndRelease(page);
```

## Internal Links

Navigate between pages within the same PDF.

### Link to Specific Page

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("multi_page.pdf", ePDFVersion13);

// Create first page
PDFPage* page1 = new PDFPage();
page1->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page1);
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

AbstractContentContext::TextOptions textOpts(font, 12, AbstractContentContext::eRGB, 0x0000FF);
ctx->WriteText(100, 700, "Go to Page 2", textOpts);

pdfWriter.EndPageContentContext(ctx);

// Write first page and get its ID
ObjectIDType page1ID = pdfWriter.WritePageAndReturnPageID(page1);
delete page1;

// Create second page
PDFPage* page2 = new PDFPage();
page2->SetMediaBox(PDFRectangle(0, 0, 595, 842));

ctx = pdfWriter.StartPageContentContext(page2);
ctx->WriteText(100, 700, "This is Page 2", textOpts);
pdfWriter.EndPageContentContext(ctx);

ObjectIDType page2ID = pdfWriter.WritePageAndReturnPageID(page2);
delete page2;

// Now add link on page 1 pointing to page 2
// Use modification mode to add link to already-written page
pdfWriter.Shutdown("multi_page.pdf");

// Reopen for modification
pdfWriter.ModifyPDF("multi_page.pdf", ePDFVersion13, "multi_page.pdf");

PDFPageInput pageInput(&pdfWriter.GetModifiedFileParser(), page1ID);
PageContentContext* modCtx = pdfWriter.StartPageContentContext(&pageInput);

// Add the internal link
// Note: Internal link creation requires lower-level APIs
// See Low-Level APIs guide for internal page navigation

pdfWriter.EndPDF();
```

**Note**: Full internal link creation requires lower-level APIs. See the [Low-Level APIs](low-level-apis.md) guide for detailed internal navigation patterns.

## Comment Annotations

Add sticky note comments to your PDF pages.

### Basic Comment

```cpp
#include "PDFComment.h"

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Draw content...

pdfWriter.EndPageContentContext(ctx);

// Create comment annotation
PDFComment comment;
comment.Text = "Review this section carefully";
comment.CommentatorName = "Editor";
comment.Time.Year = 2025;
comment.Time.Month = 1;
comment.Time.Day = 15;
comment.Time.Hour = 10;
comment.Time.Minute = 30;

// Position the comment icon
PDFRectangle commentRect(100, 700, 118, 718);  // 18x18 icon
comment.Rect = commentRect;

// Attach to current page
pdfWriter.AttachCommentToCurrentPage(comment);

pdfWriter.WritePageAndRelease(page);
```

### Comment with Color

```cpp
PDFComment comment;
comment.Text = "Important: Verify these figures";
comment.CommentatorName = "Reviewer";

// Set comment color (RGB 0-255)
comment.Color.Red = 255;
comment.Color.Green = 255;
comment.Color.Blue = 0;  // Yellow

comment.Rect = PDFRectangle(200, 500, 218, 518);

pdfWriter.AttachCommentToCurrentPage(comment);
```

### Multiple Comments

```cpp
pdfWriter.EndPageContentContext(ctx);

// Add multiple comments
PDFComment comment1;
comment1.Text = "Check formatting";
comment1.CommentatorName = "Alice";
comment1.Rect = PDFRectangle(100, 700, 118, 718);
pdfWriter.AttachCommentToCurrentPage(comment1);

PDFComment comment2;
comment2.Text = "Update contact information";
comment2.CommentatorName = "Bob";
comment2.Rect = PDFRectangle(100, 600, 118, 618);
pdfWriter.AttachCommentToCurrentPage(comment2);

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

        pdfWriter.AttachURLLinktoCurrentPage("mailto:support@example.com",
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

### Example 3: Document with Comments

```cpp
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFComment.h"
#include <iostream>

using namespace PDFHummus;

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = eSuccess;

    do {
        status = pdfWriter.StartPDF("annotated.pdf", ePDFVersion13);
        if (status != eSuccess) break;

        PDFPage* page = new PDFPage();
        page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
        PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

        // Document content
        AbstractContentContext::TextOptions textOpts(font, 12,
            AbstractContentContext::eGray, 0);

        ctx->WriteText(100, 750, "Q1 Revenue Report", textOpts);
        ctx->WriteText(100, 700, "Total Revenue: $1,234,567", textOpts);
        ctx->WriteText(100, 650, "Growth Rate: 15%", textOpts);

        pdfWriter.EndPageContentContext(ctx);

        // Add review comment
        PDFComment comment1;
        comment1.Text = "Please verify these numbers with accounting";
        comment1.CommentatorName = "Manager";
        comment1.Time.Year = 2025;
        comment1.Time.Month = 1;
        comment1.Time.Day = 15;
        comment1.Color.Red = 255;
        comment1.Color.Green = 255;
        comment1.Color.Blue = 0;  // Yellow
        comment1.Rect = PDFRectangle(80, 700, 98, 718);
        pdfWriter.AttachCommentToCurrentPage(comment1);

        // Add approval comment
        PDFComment comment2;
        comment2.Text = "Looks good, approved";
        comment2.CommentatorName = "Director";
        comment2.Time.Year = 2025;
        comment2.Time.Month = 1;
        comment2.Time.Day = 16;
        comment2.Color.Red = 0;
        comment2.Color.Green = 255;
        comment2.Color.Blue = 0;  // Green
        comment2.Rect = PDFRectangle(80, 650, 98, 668);
        pdfWriter.AttachCommentToCurrentPage(comment2);

        pdfWriter.WritePageAndRelease(page);

        status = pdfWriter.EndPDF();
        if (status != eSuccess) break;

        std::cout << "Created annotated.pdf with comments!" << std::endl;

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

### Comment Not Showing

**Problem**: Rectangle defines comment icon position, must be visible

```cpp
// Ensure comment rectangle is within page bounds
PDFRectangle commentRect(100, 700, 118, 718);  // 18x18 icon
comment.Rect = commentRect;

// Color helps visibility
comment.Color.Red = 255;
comment.Color.Green = 255;
comment.Color.Blue = 0;
```

## Advanced Annotations

For more complex annotation types, you'll need lower-level APIs:

- **Highlight annotations** - Highlight text passages
- **Shape annotations** - Circles, squares, polygons
- **File attachments** - Embed files in PDF
- **Popup annotations** - Expandable popups
- **Free text annotations** - Text boxes
- **Stamp annotations** - "Approved", "Draft" stamps

See the [Low-Level APIs](low-level-apis.md) guide for creating these advanced annotations.

## Tips and Best Practices

1. **Visual feedback** - Use blue color and underlines for clickable text
2. **Test link areas** - Verify rectangles cover the intended area
3. **Timing matters** - Always add links after `EndPageContentContext()`
4. **Comment colors** - Use colors to categorize comments (red = urgent, yellow = review, green = approved)
5. **Accessibility** - Provide alternative text for screen readers when possible
6. **Link validation** - Verify URLs are properly formatted before adding
7. **Rectangle precision** - Use `GetImageDimensions()` for exact image link areas

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
- Test file: PDFWriterTesting/PDFCommentWriter.cpp:41
