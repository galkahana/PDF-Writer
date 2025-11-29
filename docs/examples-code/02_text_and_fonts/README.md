# Text and Fonts Examples

Examples demonstrating text rendering and font usage in PDF-Writer.

## Prerequisites

These examples require a TrueType font file. Create a `fonts/` directory and add `arial.ttf`:

```bash
mkdir fonts

# On Windows:
# copy C:\Windows\Fonts\arial.ttf fonts\

# On macOS:
# cp /System/Library/Fonts/Supplemental/Arial.ttf fonts/arial.ttf

# On Linux:
# cp /usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf fonts/arial.ttf
```

Or use any .ttf font file you have available.

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Examples

### simple_text

Demonstrates basic text rendering:
- Loading fonts with GetFontForFile()
- Using WriteText() high-level API
- Different colors (black, red, blue, gray)
- Different font sizes
- Opacity/transparency
- Multi-line text with manual positioning

```bash
./simple_text
```

Output: `simple_text.pdf`

### multiline_text

Demonstrates multi-line text layout:
- BT/ET text object operators
- Tm transformation matrix
- Td text positioning
- Line spacing (leading)
- Different font sizes in same document
- Using std::vector for paragraph text

```bash
./multiline_text
```

Output: `multiline_text.pdf`

## What You'll Learn

- How to load and use TrueType fonts
- High-level vs mid-level text APIs
- Text positioning and coordinate system
- Color and opacity control
- Multi-line text layout techniques
- Line spacing (leading)

## See Also

- [Documentation: Text & Fonts](../../guides/text-and-fonts.md)
- [Documentation: Core Concepts](../../getting-started/core-concepts.md)
- Test file: PDFWriterTesting/SimpleTextUsage.cpp
- Test file: PDFWriterTesting/HighLevelContentContext.cpp
