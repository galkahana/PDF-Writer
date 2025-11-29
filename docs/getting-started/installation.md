# Installation

This guide shows you how to set up PDF-Writer in your C++ project.

## Prerequisites

- **Compiler**: Visual Studio (Windows), GCC, or Clang (Linux/macOS)
- **CMake**: 3.10 or later ([download](https://cmake.org/))

**That's it!** All other dependencies (FreeType, Zlib, LibPng, LibTiff, LibJpeg, LibAesgm) are bundled with the library.

**Optional**: OpenSSL (only for PDF 2.0 encryption features - PDF 1.7 and earlier encryption works without it)

## Quick Start

If you just want to try PDF-Writer quickly:

```bash
git clone https://github.com/galkahana/PDF-Writer.git
cd PDF-Writer
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Run tests to verify
ctest --test-dir . -C Release
```

Test outputs will be in `./Testing/Output/` - you should see generated PDFs!

## Integrating into Your Project

### Option 1: CMake FetchContent (Recommended)

This is the easiest way to add PDF-Writer to a CMake project:

```cmake
cmake_minimum_required(VERSION 3.14)
project(MyPDFApp)

include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.8.0  # Use latest release
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)

add_executable(myapp main.cpp)
target_link_libraries(myapp PDFHummus::PDFWriter)
```

**Pros**:
- Automatic download and build
- Always uses compatible version
- Easy version updates

**Cons**:
- Slower initial build (downloads and builds dependency)

### Option 2: find_package

If you've installed PDF-Writer system-wide or in a known location:

```cmake
find_package(PDFHummus REQUIRED)
target_link_libraries(myapp PDFHummus::PDFWriter)
```

To install PDF-Writer for find_package:

```bash
cd PDF-Writer
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local  # or your preferred location
cmake --build . --config Release
cmake --install . --config Release
```

**Pros**:
- Fast builds (uses pre-built library)
- Shared across projects

**Cons**:
- Manual installation step
- Need to manage versions yourself

### Option 3: Copy Sources

For simple projects or when you want full control:

1. Copy these folders to your project:
   - `PDFWriter/` (core library)
   - `FreeType/`, `Zlib/`, `LibJpeg/`, `LibPng/`, `LibTiff/`, `LibAesgm/` (dependencies)

2. Add to your build system (CMake example):

```cmake
add_subdirectory(PDF-Writer)
target_link_libraries(myapp PDFHummus::PDFWriter)
```

**Pros**:
- Complete control
- Easy to debug and modify
- No external dependencies

**Cons**:
- Larger repository
- Manual updates

## Bundled Dependencies

**PDF-Writer bundles all required libraries** for portability and easy setup:

- **FreeType** - Font rendering (always included)
- **Zlib** - Compression (always included)
- **LibPng** - PNG image support (included by default)
- **LibTiff** - TIFF image support (included by default)
- **LibJpeg** - JPEG decoding for parsing (included by default)
- **LibAesgm** - AES encryption (included by default)

**OpenSSL** is the only external dependency (required only for PDF 2.0 encryption features).

This bundling ensures consistent behavior across platforms and eliminates dependency management headaches.

## Customizing the Build

You can disable optional features if needed:

### Disabling Image Format Support

**JPEG embedding** is native to PDF and always available. The flags below control parsing/decoding and other formats:

```bash
# Disable JPEG decoding (for parsing existing PDFs with JPEGs)
# Note: JPEG embedding still works (native PDF support)
cmake .. -DPDFHUMMUS_NO_DCT=TRUE

# Disable PNG support (removes bundled LibPng)
cmake .. -DPDFHUMMUS_NO_PNG=TRUE

# Disable TIFF support (removes bundled LibTiff)
cmake .. -DPDFHUMMUS_NO_TIFF=TRUE
```

In FetchContent:

```cmake
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.8.0
)
# Disable PNG if not needed
set(PDFHUMMUS_NO_PNG TRUE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(PDFHummus)
```

**When to disable**:
- Building for embedded systems with size constraints
- Only using JPEG images
- Reducing attack surface for security-sensitive applications

### Disabling PDF 2.0 Encryption

OpenSSL is required only for PDF 2.0 encryption features. PDF 1.7 and earlier encryption works natively.

```bash
# Disable OpenSSL dependency (removes PDF 2.0 encryption only)
cmake .. -DPDFHUMMUS_NO_OPENSSL=TRUE
```

Or in CMake:

```cmake
set(PDFHUMMUS_NO_OPENSSL TRUE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(PDFHummus)
```

**Note**: Standard encryption (RC4, AES-128, AES-256 for PDF 1.7 and earlier) still works without OpenSSL.

### Using System Libraries

By default, PDF-Writer uses bundled dependencies. To use system libraries:

```bash
cmake .. -DUSE_BUNDLED=FALSE
```

Or with fallback to bundled if system libs aren't found:

```bash
cmake .. -DUSE_UNBUNDLED_FALLBACK_BUNDLED=TRUE
```

## Platform-Specific Notes

### Windows (Visual Studio)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

The Visual Studio solution will be in the `build/` directory.

### macOS

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

For Xcode projects:

```bash
cmake .. -G Xcode
```

### Linux

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release -j$(nproc)
```

### iOS

PDF-Writer supports iOS. See the [blog post](http://pdfhummus.com/post/45501609236/how-to-build-iphone-apps-that-use-pdfhummus) for detailed instructions.

## Verifying Your Installation

Create a simple test file to verify everything works:

**test.cpp**:
```cpp
#include "PDFWriter.h"
#include <iostream>

int main() {
    PDFWriter pdfWriter;
    EStatusCode status = pdfWriter.StartPDF("test.pdf", ePDFVersion13);

    if (status != eSuccess) {
        std::cout << "Failed to create PDF" << std::endl;
        return 1;
    }

    pdfWriter.EndPDF();
    std::cout << "Successfully created test.pdf!" << std::endl;
    return 0;
}
```

Build and run:

```bash
# If using CMake
cmake --build . --config Release
./myapp

# You should see: "Successfully created test.pdf!"
# And test.pdf should exist in your directory
```

## Troubleshooting

### "Cannot find PDFWriter.h"

Make sure you're linking against `PDFHummus::PDFWriter`:

```cmake
target_link_libraries(myapp PDFHummus::PDFWriter)
```

### "Undefined reference to PDFWriter::StartPDF"

Your linker can't find the library. Check:
- You're linking with `PDFHummus::PDFWriter`
- The library was built successfully
- You're using the correct build configuration (Release vs Debug)

### OpenSSL Errors on macOS

If you see OpenSSL-related errors and don't need encryption:

```bash
cmake .. -DPDFHUMMUS_NO_OPENSSL=TRUE
```

Or install OpenSSL via Homebrew:

```bash
brew install openssl
cmake .. -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl
```

### Build Errors with Optional Dependencies

If you see errors related to LibJpeg, LibPng, or LibTiff and don't need those formats:

```bash
cmake .. -DPDFHUMMUS_NO_DCT=TRUE -DPDFHUMMUS_NO_PNG=TRUE -DPDFHUMMUS_NO_TIFF=TRUE
```

## Next Steps

Now that PDF-Writer is installed, create your first PDF:

→ **[Your First PDF](first-pdf.md)** - Create a PDF in 5 minutes

## Complete CMake Options Reference

| Option | Default | Description |
|--------|---------|-------------|
| `PDFHUMMUS_NO_DCT` | FALSE | Disable JPEG support (no LibJpeg) |
| `PDFHUMMUS_NO_TIFF` | FALSE | Disable TIFF support (no LibTiff) |
| `PDFHUMMUS_NO_PNG` | FALSE | Disable PNG support (no LibPng) |
| `PDFHUMMUS_NO_OPENSSL` | FALSE | Disable PDF 2.0 encryption (no OpenSSL) |
| `USE_BUNDLED` | TRUE | Use bundled dependencies |
| `USE_UNBUNDLED_FALLBACK_BUNDLED` | FALSE | Try system libs, fallback to bundled |
| `BUILD_FUZZING_HARNESS` | FALSE | Enable fuzz testing (for developers) |
