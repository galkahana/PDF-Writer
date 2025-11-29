# Image Examples

Examples demonstrating image embedding in PDF-Writer.

## Prerequisites

You'll need a JPEG image file for testing. Name it `sample.jpg` and place it in the build directory.

You can use any JPEG file - a photo, logo, or download a sample:
```bash
# Example: Download a sample image (requires curl)
curl -o sample.jpg https://via.placeholder.com/400x300.jpg
```

Or just copy any .jpg file you have:
```bash
# On Linux/macOS
cp /path/to/your/photo.jpg sample.jpg

# On Windows
copy C:\path\to\your\photo.jpg sample.jpg
```

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Make sure sample.jpg is in the build directory
cp ../path/to/sample.jpg .
```

## Running

```bash
./simple_image
```

This will create `simple_image.pdf` showing the same image in 4 different ways:
1. **Native size** - Image at original dimensions
2. **Scaled 50%** - Using matrix transformation
3. **Fit proportional** - Fit to 200x150 box, maintaining aspect ratio
4. **Fit stretched** - Fit to 200x150 box, stretching to fill

## What You'll Learn

- Using `DrawImage()` to place images
- Getting image dimensions with `GetImageDimensions()`
- Matrix transformation for scaling
- Fit transformation with and without proportional scaling
- Image positioning on the page

## See Also

- [Documentation: Images](../../guides/images.md)
- Test file: PDFWriterTesting/HighLevelImages.cpp
- Test file: PDFWriterTesting/JPGImageTest.cpp
