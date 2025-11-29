# Images

Learn how to embed images in your PDFs using JPEG, PNG, and TIFF formats.

## Quick Start

The simplest way to add an image:

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Place a JPEG image at position (100, 500)
ctx->DrawImage(100, 500, "photo.jpg");

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

## Supported Image Formats

| Format | Built-in | Transparency | Compression | Notes |
|--------|----------|--------------|-------------|-------|
| **JPEG** | ✅ Always | ❌ No | Lossy | Photos, complex images |
| **PNG** | ✅ Default | ✅ Yes | Lossless | Screenshots, diagrams, transparency |
| **TIFF** | ✅ Default | ✅ Yes (some modes) | Lossless | Scans, multi-page, technical images |

**All libraries are bundled** - PDF-Writer includes LibPng, LibTiff, FreeType, and Zlib by default. This ensures portability and consistent behavior across platforms.

### JPEG

Native PDF format, always supported:
- **Efficient**: JPEG data embedded directly (no re-encoding)
- **Lossy compression**: Great for photos
- **No transparency**: Opaque images only
- **Color spaces**: RGB, CMYK, Grayscale
- **Best for**: Photos, complex images

### PNG

Bundled LibPng support (enabled by default):
- **Lossless compression**: Preserves image quality
- **Transparency**: Full alpha channel support
- **Best for**: Screenshots, diagrams, logos, images requiring transparency
- **Disable**: Build with `-DPDFHUMMUS_NO_PNG=TRUE` if not needed

```cpp
// Works by default (LibPng bundled)
ctx->DrawImage(100, 400, "screenshot.png");

// Optional: wrap for builds with PNG disabled
#ifndef PDFHUMMUS_NO_PNG
ctx->DrawImage(100, 400, "screenshot.png");
#endif
```

### TIFF

Bundled LibTiff support (enabled by default):
- **Lossless compression**: Various compression schemes
- **Multi-page**: Single file with multiple images
- **Transparency**: Supported in some modes
- **Color modes**: B&W, Grayscale, RGB, CMYK, Palette
- **Best for**: Scanned documents, technical/medical images
- **Disable**: Build with `-DPDFHUMMUS_NO_TIFF=TRUE` if not needed

```cpp
// Works by default (LibTiff bundled)
ctx->DrawImage(100, 200, "scan.tif");

// Optional: wrap for builds with TIFF disabled
#ifndef PDFHUMMUS_NO_TIFF
ctx->DrawImage(100, 200, "scan.tif");
#endif
```

## Drawing Images

### Basic Placement

```cpp
// Place image at (x, y) position at native size
ctx->DrawImage(100, 500, "photo.jpg");
```

The position (100, 500) is the **bottom-left corner** of the image (remember: bottom-left origin coordinate system).

### Scaling with Matrix Transform

```cpp
AbstractContentContext::ImageOptions opts;
opts.transformationMethod = AbstractContentContext::eMatrix;

// Scale to 50% of original size
opts.matrix[0] = 0.5;  // Scale X
opts.matrix[3] = 0.5;  // Scale Y

ctx->DrawImage(100, 500, "photo.jpg", opts);
```

**Transformation matrix**: `[scaleX, skewY, skewX, scaleY, translateX, translateY]`
- Default (identity): `[1, 0, 0, 1, 0, 0]`
- Scale 2x: `[2, 0, 0, 2, 0, 0]`
- Scale 50%: `[0.5, 0, 0, 0.5, 0, 0]`

### Fit to Bounding Box

```cpp
AbstractContentContext::ImageOptions opts;
opts.transformationMethod = AbstractContentContext::eFit;
opts.boundingBoxWidth = 400;
opts.boundingBoxHeight = 300;
opts.fitProportional = true;  // Maintain aspect ratio

ctx->DrawImage(100, 400, "photo.jpg", opts);
```

**Fit policies**:
- `fitProportional = true`: Maintains aspect ratio (recommended)
- `fitProportional = false`: Stretches to fill box
- `fitPolicy = eOverflow`: Only scales down if image too large
- `fitPolicy = eAlways`: Always scales to fit box

## Getting Image Dimensions

Calculate dimensions before placement:

```cpp
DoubleAndDoublePair dims = pdfWriter.GetImageDimensions("photo.jpg");

double width = dims.first;
double height = dims.second;

std::cout << "Image size: " << width << " x " << height << std::endl;

// Center image on page
double pageWidth = 595;   // A4 width
double pageHeight = 842;  // A4 height

double x = (pageWidth - width) / 2;
double y = (pageHeight - height) / 2;

ctx->DrawImage(x, y, "photo.jpg");
```

## Complete Examples

### Example 1: Photo with Proportional Scaling

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("photo_page.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Fit photo to 400x400 area, maintaining aspect ratio
AbstractContentContext::ImageOptions opts;
opts.transformationMethod = AbstractContentContext::eFit;
opts.boundingBoxWidth = 400;
opts.boundingBoxHeight = 400;
opts.fitProportional = true;

ctx->DrawImage(100, 350, "vacation.jpg", opts);

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

### Example 2: Multiple Images on One Page

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("gallery.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Create options for thumbnails (150x150 max)
AbstractContentContext::ImageOptions thumbOpts;
thumbOpts.transformationMethod = AbstractContentContext::eFit;
thumbOpts.boundingBoxWidth = 150;
thumbOpts.boundingBoxHeight = 150;
thumbOpts.fitProportional = true;

// Place images in grid
ctx->DrawImage(50, 650, "photo1.jpg", thumbOpts);
ctx->DrawImage(220, 650, "photo2.jpg", thumbOpts);
ctx->DrawImage(390, 650, "photo3.jpg", thumbOpts);

ctx->DrawImage(50, 450, "photo4.jpg", thumbOpts);
ctx->DrawImage(220, 450, "photo5.jpg", thumbOpts);
ctx->DrawImage(390, 450, "photo6.jpg", thumbOpts);

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

### Example 3: Image with Text Caption

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("captioned.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

// Place image
AbstractContentContext::ImageOptions imgOpts;
imgOpts.transformationMethod = AbstractContentContext::eFit;
imgOpts.boundingBoxWidth = 400;
imgOpts.boundingBoxHeight = 300;
imgOpts.fitProportional = true;

ctx->DrawImage(100, 450, "diagram.jpg", imgOpts);

// Add caption below image
AbstractContentContext::TextOptions textOpts(font, 11, eGray, 0);
ctx->WriteText(100, 430, "Figure 1: System Architecture Diagram", textOpts);

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

## Multi-Page TIFF

TIFF files can contain multiple pages/images:

```cpp
#ifndef PDFHUMMUS_NO_TIFF
// Get number of pages in TIFF
unsigned long pageCount = pdfWriter.GetImagePagesCount("multipage.tif");

// Place specific page (zero-indexed)
AbstractContentContext::ImageOptions opts;
opts.imageIndex = 0;  // First page
ctx->DrawImage(100, 500, "multipage.tif", opts);

opts.imageIndex = 1;  // Second page
ctx->DrawImage(100, 300, "multipage.tif", opts);
#endif
```

## Loading Images from Memory

For images loaded from memory, network, or database:

```cpp
#include "InputFile.h"

// Open image file as stream
InputFile imageFile;
EStatusCode status = imageFile.OpenFile("photo.jpg");
if (status != eSuccess) {
    std::cout << "Failed to open image file" << std::endl;
    return 1;
}

// Create form XObject from stream
PDFFormXObject* formXObject =
    pdfWriter.CreateFormXObjectFromJPGStream(imageFile.GetInputStream());

imageFile.CloseFile();

if (!formXObject) {
    std::cout << "Failed to create form XObject" << std::endl;
    return 1;
}

// Use the form XObject
PageContentContext* ctx = pdfWriter.StartPageContentContext(page);
ctx->q();
ctx->cm(1, 0, 0, 1, 100, 500);  // Position at (100, 500)
ctx->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
ctx->Q();

delete formXObject;
```

**Note**: Any class implementing `IByteReaderWithPosition` can be used as a stream source.

## Image Type Detection

Automatically determine image format:

```cpp
EHummusImageType imageType = pdfWriter.GetImageType("unknown.dat", 0);

switch(imageType) {
    case eJPG:
        std::cout << "JPEG image" << std::endl;
        break;
    case ePNG:
        std::cout << "PNG image" << std::endl;
        break;
    case eTIFF:
        std::cout << "TIFF image" << std::endl;
        break;
    case ePDF:
        std::cout << "PDF document" << std::endl;
        break;
    default:
        std::cout << "Unknown format" << std::endl;
}
```

## Making Images Clickable

Combine images with URL links (see [Links & Annotations](links-and-annotations.md)):

```cpp
// Place image
DoubleAndDoublePair dims = pdfWriter.GetImageDimensions("logo.jpg");
ctx->DrawImage(100, 700, "logo.jpg");

// Add clickable link over the image
PDFRectangle linkArea(100, 700, 100 + dims.first, 700 + dims.second);
// See Links & Annotations guide for complete link creation
```

## Common Issues

### Image Not Appearing

**Problem**: Image placed outside page bounds
```cpp
// WRONG - Y=1000 is above A4 page height (842)
ctx->DrawImage(100, 1000, "photo.jpg");

// CORRECT - Within page bounds
ctx->DrawImage(100, 500, "photo.jpg");
```

### Image Too Large

**Problem**: Image extends beyond page

**Solution**: Use fit transformation
```cpp
// Get image size
DoubleAndDoublePair dims = pdfWriter.GetImageDimensions("huge.jpg");

if (dims.first > 500 || dims.second > 500) {
    // Scale to fit 500x500 area
    AbstractContentContext::ImageOptions opts;
    opts.transformationMethod = AbstractContentContext::eFit;
    opts.boundingBoxWidth = 500;
    opts.boundingBoxHeight = 500;
    opts.fitProportional = true;
    ctx->DrawImage(50, 300, "huge.jpg", opts);
} else {
    // Use native size
    ctx->DrawImage(50, 300, "huge.jpg");
}
```

### PNG/TIFF Not Working

**Problem**: Library not compiled with PNG/TIFF support

**Solution**: Wrap in conditional compilation or use JPEG
```cpp
#ifndef PDFHUMMUS_NO_PNG
ctx->DrawImage(100, 400, "image.png");
#else
// Fallback: convert to JPEG first or show error
std::cout << "PNG support not available" << std::endl;
#endif
```

### Image Appears Distorted

**Problem**: Non-proportional scaling

**Solution**: Use `fitProportional = true`
```cpp
AbstractContentContext::ImageOptions opts;
opts.transformationMethod = AbstractContentContext::eFit;
opts.fitProportional = true;  // Maintains aspect ratio
opts.boundingBoxWidth = 300;
opts.boundingBoxHeight = 300;
```

## Tips and Best Practices

1. **Use JPEG for portability** - No external dependencies required
2. **Always check dimensions** - Use `GetImageDimensions()` before placement
3. **Use proportional fitting** - Prevents distortion
4. **Mind the coordinate system** - Image position is bottom-left corner
5. **Wrap PNG/TIFF** - Always use `#ifndef PDFHUMMUS_NO_PNG` checks
6. **Reuse images** - DrawImage() automatically reuses image resources
7. **Check file paths** - Use absolute paths or verify relative path resolution

## Next Steps

→ **[Links & Annotations](links-and-annotations.md)** - Make images clickable
→ **[Graphics & Drawing](graphics-and-drawing.md)** - Draw shapes around images
→ **[PDF-to-PDF Operations](pdf-to-pdf.md)** - Use PDF pages as images

## See Also

- [Core Concepts](../getting-started/core-concepts.md#coordinate-system) - Coordinate system
- [Example Code](../examples-code/03_images/) - Compilable image examples
- Test file: PDFWriterTesting/HighLevelImages.cpp:55
- Test file: PDFWriterTesting/JPGImageTest.cpp:76
