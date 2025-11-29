# Text & Fonts

Learn how to add text to your PDFs using different fonts, styles, and layouts.

## Quick Start

The simplest way to add text to a PDF:

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("output.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

// Load a font
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

// Write text using high-level API
AbstractContentContext::TextOptions textOptions(
    font,
    14,                                      // Font size
    AbstractContentContext::eGray,           // Color space
    0                                        // Color (black)
);

ctx->WriteText(75, 700, "Hello World!", textOptions);

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

## Loading Fonts

PDF-Writer supports multiple font formats through FreeType integration:

### TrueType Fonts (.ttf)

Most common font format:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
```

**Examples**: Arial, Times New Roman, Verdana

### OpenType Fonts (.otf)

Modern font format supporting both TrueType and PostScript outlines:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/BrushScriptStd.otf");
```

**Examples**: Adobe fonts, Google Fonts

### Type 1 Fonts (.pfb + .pfm)

PostScript Type 1 fonts require both the font program (.pfb) and metrics (.pfm) files:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile(
    "fonts/HelveticaLT.pfb",
    "fonts/HelveticaLT.pfm"
);
```

**Note**: Type 1 is an older format, use TrueType/OpenType when possible.

### TrueType Collections (.ttc)

Multiple fonts in a single file:

```cpp
// Font index 0 (usually Regular)
PDFUsedFont* regular = pdfWriter.GetFontForFile("fonts/HelveticaNeue.ttc", 0);

// Font index 1 (usually Bold)
PDFUsedFont* bold = pdfWriter.GetFontForFile("fonts/HelveticaNeue.ttc", 1);
```

**Examples**: System fonts on macOS, Asian font collections

### DFont Format (.dfont)

Mac-specific font format:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/Courier.dfont", 0);
```

### Error Handling

**Always check for NULL** - font loading can fail:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
if (!font) {
    std::cout << "Failed to load font!" << std::endl;
    return 1;
}
```

**Common reasons for failure**:
- File doesn't exist or path is wrong
- File is corrupted
- Unsupported font format
- Insufficient memory

## Font Embedding

Control whether fonts are embedded in the PDF:

```cpp
PDFCreationSettings settings(
    true,   // Compress streams
    true    // Embed fonts (false = reference only)
);

pdfWriter.StartPDF("output.pdf", ePDFVersion13,
    LogConfiguration::DefaultLogConfiguration(),
    settings);
```

**Embedded fonts** (recommended):
- ✅ PDF displays correctly on any system
- ✅ Text always looks the same
- ❌ Larger file size

**Referenced fonts**:
- ✅ Smaller file size
- ❌ Requires font on viewer's system
- ❌ May display incorrectly if font missing

**Default**: Fonts are embedded.

## Writing Text

### High-Level API: WriteText()

Best for most use cases:

```cpp
AbstractContentContext::TextOptions textOptions(
    font,           // PDFUsedFont* from GetFontForFile()
    14,             // Font size in points
    AbstractContentContext::eGray,  // Color space
    0,              // Color value (black)
    1.0             // Opacity (1.0 = opaque, optional)
);

ctx->WriteText(100, 700, "Your text here", textOptions);
```

**Parameters**:
- `inX`, `inY`: Position (bottom-left of text baseline)
- `inText`: UTF-8 encoded string
- `inOptions`: TextOptions with font, size, color

**Returns**: `eSuccess` or `eFailure` (if glyphs not found in font)

### Color Options

**Grayscale**:
```cpp
TextOptions(font, 14, AbstractContentContext::eGray, 0);  // Black (0 = black, 255 = white)
```

**RGB**:
```cpp
TextOptions(font, 14, AbstractContentContext::eRGB, 0xFF0000);  // Red
TextOptions(font, 14, AbstractContentContext::eRGB, 0x00FF00);  // Green
TextOptions(font, 14, AbstractContentContext::eRGB, 0x0000FF);  // Blue
```

**CMYK**:
```cpp
// Format: 0xCCMMYYKK (each component 0-255)
TextOptions(font, 14, AbstractContentContext::eCMYK, 0xFF000000);  // Cyan
```

**Named colors**:
```cpp
unsigned long color = AbstractContentContext::ColorValueForName("DarkMagenta");
TextOptions(font, 14, AbstractContentContext::eRGB, color);
```

### Opacity (Transparency)

```cpp
// 50% transparent text
TextOptions(font, 14, AbstractContentContext::eRGB, 0x000000, 0.5);

// Fully opaque (default)
TextOptions(font, 14, AbstractContentContext::eRGB, 0x000000, 1.0);
```

## Text Positioning

PDF uses a **bottom-left origin** coordinate system (see [Core Concepts](../getting-started/core-concepts.md#coordinate-system) for details).

```cpp
// Near top of A4 page
ctx->WriteText(100, 750, "Near top", textOptions);

// Near bottom
ctx->WriteText(100, 50, "Near bottom", textOptions);
```

**Key points**:
- Coordinates are at the text **baseline** (bottom of most letters), not top
- Position is at the **left edge** of the text
- Y increases upward (0 is at bottom of page)

## Unicode and International Text

All text methods accept **UTF-8 encoded strings**:

```cpp
// English
ctx->WriteText(100, 700, "Hello World", textOptions);

// Japanese
ctx->WriteText(100, 650, "こんにちは世界", textOptions);

// Hebrew
ctx->WriteText(100, 600, "שלום עולם", textOptions);

// Mixed
ctx->WriteText(100, 550, "Hello 世界 مرحبا", textOptions);
```

**Requirements**:
- Font must contain the glyphs for your characters
- Use fonts with broad Unicode coverage for international text
- CID fonts (Asian languages) are automatically handled

**C++ UTF-8 string literals**:

```cpp
// Modern C++ (C++11+)
ctx->WriteText(100, 700, u8"Hello 世界", textOptions);

// Or use hex escapes
ctx->WriteText(100, 700, "hello \xe6\xb8\xb8\xe7\xaf\x89", textOptions);
```

## Emoji Support

PDF-Writer supports color emoji through OpenType COLR tables:

```cpp
// Load emoji font
PDFUsedFont* emojiFont = pdfWriter.GetFontForFile("fonts/seguiemj.ttf");

AbstractContentContext::TextOptions emojiOptions(
    emojiFont,
    24,
    AbstractContentContext::eGray,
    0
);

// UTF-8 emoji
ctx->WriteText(100, 500, "😀😃😄😁", emojiOptions);
ctx->WriteText(100, 450, "☺☺☺", emojiOptions);
```

**Supported emoji fonts**:
- Segoe UI Emoji (Windows): seguiemj.ttf
- Noto Color Emoji (Google): NotoColorEmoji.ttf
- Apple Color Emoji (macOS): AppleColorEmoji.ttc

**Note**: Emoji rendering requires fonts with COLR or COLR v1 tables. WriteText() automatically detects and renders color glyphs.

## Multi-Line Text

Use Td() to move to the next line:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
double fontSize = 12;
double leading = 14;  // Line spacing

ctx->BT();                                    // Begin text object
ctx->Tf(font, 1);                            // Set font
ctx->k(0, 0, 0, 1);                          // Black color (CMYK)

// First line
ctx->Tm(fontSize, 0, 0, fontSize, 50, 700);
ctx->Tj("Line 1: First line of text");

// Move down by leading amount
ctx->Td(0, -leading);
ctx->Tj("Line 2: Second line");

ctx->Td(0, -leading);
ctx->Tj("Line 3: Third line");

ctx->ET();                                    // End text object
```

**Key operators**:
- `BT()` - Begin text object
- `ET()` - End text object
- `Tf(font, size)` - Set font and size
- `Tm(...)` - Set transformation matrix (position + scale)
- `Td(dx, dy)` - Move to next line
- `Tj(text)` - Show text

## Text Measurement

Calculate text dimensions before rendering:

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
std::string text = "Measure me!";
double fontSize = 14;

// Get bounding box
PDFUsedFont::TextMeasures dims = font->CalculateTextDimensions(text, fontSize);

std::cout << "Width: " << dims.width << std::endl;
std::cout << "Height: " << dims.height << std::endl;
std::cout << "BBox: (" << dims.xMin << ", " << dims.yMin << ") to ("
          << dims.xMax << ", " << dims.yMax << ")" << std::endl;

// Or just get width
double width = font->CalculateTextAdvance(text, fontSize);
```

**Use cases**:
- Right-align text
- Center text
- Draw bounding boxes
- Calculate page layout

### Right-Aligned Text

```cpp
std::string text = "Right aligned text";
double fontSize = 14;
double rightMargin = 545;  // Page width 595 - 50 margin

double textWidth = font->CalculateTextAdvance(text, fontSize);

AbstractContentContext::TextOptions options(font, fontSize, eGray, 0);
ctx->WriteText(rightMargin - textWidth, 700, text, options);
```

### Centered Text

```cpp
std::string text = "Centered text";
double fontSize = 14;
double pageWidth = 595;

double textWidth = font->CalculateTextAdvance(text, fontSize);
double x = (pageWidth - textWidth) / 2;

ctx->WriteText(x, 400, text, options);
```

## Advanced Text Control

For precise control, use PDF text operators directly:

### Manual Text Positioning

```cpp
ctx->BT();                              // Begin text
ctx->Tf(font, 1);                       // Set font (size multiplied by matrix)
ctx->k(0, 0, 0, 1);                     // Set color

// Transformation matrix: scale and position
// Tm(a, b, c, d, e, f) where:
//   a,d = scale, b,c = skew, e,f = translation
ctx->Tm(20, 0, 0, 20, 100, 500);       // 20pt font at (100, 500)

ctx->Tj("Hello World");                 // Show text

ctx->ET();                              // End text
```

### Text with Kerning (TJ operator)

For precise character spacing:

```cpp
ctx->BT();
ctx->Tf(font, 1);
ctx->Tm(20, 0, 0, 20, 50, 500);

StringOrDoubleList textAndKerning;
textAndKerning.push_back(StringOrDouble("AV"));   // Letters to kern
textAndKerning.push_back(-80.0);                  // Negative = tighten
textAndKerning.push_back(StringOrDouble("A"));

ctx->TJ(textAndKerning);
ctx->ET();
```

**Note**: Kerning values are in 1/1000ths of an em. Negative values move characters closer.

### Text State Operators

```cpp
ctx->Tc(0.1);           // Character spacing (points)
ctx->Tw(0.5);           // Word spacing (points)
ctx->Tz(120);           // Horizontal scaling (100 = normal, 120 = 120%)
ctx->TL(14);            // Text leading (line spacing)
ctx->Tr(1);             // Rendering mode (0=fill, 1=stroke, 2=fill+stroke, etc.)
ctx->Ts(5);             // Text rise (superscript/subscript offset)
```

## Complete Examples

### Example 1: Simple Styled Text

```cpp
PDFWriter pdfWriter;
pdfWriter.StartPDF("styled_text.pdf", ePDFVersion13);

PDFPage* page = new PDFPage();
page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

PageContentContext* ctx = pdfWriter.StartPageContentContext(page);

PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");

// Title (large, red)
AbstractContentContext::TextOptions titleOpts(
    font, 24, AbstractContentContext::eRGB, 0xFF0000);
ctx->WriteText(100, 750, "Document Title", titleOpts);

// Body (normal, black)
AbstractContentContext::TextOptions bodyOpts(
    font, 12, AbstractContentContext::eGray, 0);
ctx->WriteText(100, 700, "This is the body text.", bodyOpts);

pdfWriter.EndPageContentContext(ctx);
pdfWriter.WritePageAndRelease(page);
pdfWriter.EndPDF();
```

### Example 2: Multi-Line Paragraph

```cpp
PDFUsedFont* font = pdfWriter.GetFontForFile("fonts/arial.ttf");
double fontSize = 11;
double leading = 13;
double x = 50;
double y = 750;

std::vector<std::string> lines = {
    "This is a paragraph of text.",
    "Each line is positioned manually.",
    "Leading controls the spacing between lines.",
    "This creates a readable paragraph layout."
};

ctx->BT();
ctx->Tf(font, 1);
ctx->k(0, 0, 0, 1);

for (size_t i = 0; i < lines.size(); ++i) {
    if (i == 0) {
        ctx->Tm(fontSize, 0, 0, fontSize, x, y);
    } else {
        ctx->Td(0, -leading);
    }
    ctx->Tj(lines[i]);
}

ctx->ET();
```

### Example 3: Mixed Fonts

```cpp
PDFUsedFont* regular = pdfWriter.GetFontForFile("fonts/arial.ttf");
PDFUsedFont* bold = pdfWriter.GetFontForFile("fonts/arialbd.ttf");

AbstractContentContext::TextOptions regularOpts(regular, 12, eGray, 0);
AbstractContentContext::TextOptions boldOpts(bold, 12, eGray, 0);

double y = 700;

ctx->WriteText(100, y, "This is ", regularOpts);
double width1 = regular->CalculateTextAdvance("This is ", 12);

ctx->WriteText(100 + width1, y, "bold text", boldOpts);
double width2 = bold->CalculateTextAdvance("bold text", 12);

ctx->WriteText(100 + width1 + width2, y, " in a sentence.", regularOpts);
```

## Common Issues

### Text Not Appearing

**Problem**: Text is positioned off the page
```cpp
// WRONG - text is above the page
ctx->WriteText(100, 900, "Hidden text", opts);

// CORRECT - within A4 page bounds (0-842)
ctx->WriteText(100, 700, "Visible text", opts);
```

### Font Not Found

**Problem**: GetFontForFile() returns NULL

**Solution**:
- Check file path is correct
- Use absolute paths or test helper functions
- Verify font file exists and is readable

### Missing Glyphs

**Problem**: WriteText() returns `eFailure`

**Solution**:
- Font doesn't contain the characters you're trying to render
- Use a font with broader Unicode coverage
- Check that font supports your language

### BT/ET Not Paired

**Problem**: PDF is corrupted or text doesn't appear

**Solution**:
```cpp
// CORRECT
ctx->BT();
// ... text operations ...
ctx->ET();

// WRONG - missing ET()
ctx->BT();
ctx->Tj("text");
// Missing ctx->ET() !
```

### Text Overlapping

**Problem**: Multiple text lines at same position

**Solution**: Use Td() or recalculate Y position:
```cpp
// Move down between lines
ctx->Td(0, -14);

// Or recalculate position
ctx->WriteText(100, 700, "Line 1", opts);
ctx->WriteText(100, 686, "Line 2", opts);  // 700 - 14
```

## Tips and Best Practices

1. **Use WriteText() for simple cases** - It handles BT/ET and color setup automatically
2. **Check font loading** - Always verify GetFontForFile() doesn't return NULL
3. **Measure before centering** - Use CalculateTextAdvance() for alignment
4. **Embed fonts by default** - Ensures consistent rendering across systems
5. **Use UTF-8** - All text methods accept UTF-8 encoded strings
6. **Test with your fonts** - Different fonts render differently
7. **Mind the coordinate system** - Y increases upward from bottom
8. **Save graphics state** - Use q()/Q() when changing text properties temporarily

## Next Steps

Now that you can add text, learn about:

→ **[Images](images.md)** - Embed JPEG, PNG, and TIFF images
→ **[Links](links.md)** - Make text clickable
→ **[Graphics & Drawing](graphics-and-drawing.md)** - Draw shapes and lines

## See Also

- [Core Concepts](../getting-started/core-concepts.md) - Coordinate system and architecture
- [Example Code](../examples-code/02_text_and_fonts/) - Compilable text examples
- Test file: PDFWriterTesting/SimpleTextUsage.cpp:34
- Test file: PDFWriterTesting/HighLevelContentContext.cpp:115
- Test file: PDFWriterTesting/UnicodeTextUsage.cpp:28
