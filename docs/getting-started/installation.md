# Installation

This guide shows you how to set up PDF-Writer in your C++ project.

## Prerequisites

- **Compiler**: Visual Studio (Windows), GCC, or Clang (Linux/macOS)
- **CMake**: 3.10 or later ([download](https://cmake.org/))
- **Optional**: OpenSSL (for PDF 2.0 encryption features)

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

## Customizing the Build

PDF-Writer has optional features you can enable/disable:

### Disabling Image Format Support

If you don't need certain image formats, you can reduce dependencies:

```bash
# Disable JPEG support
cmake .. -DPDFHUMMUS_NO_DCT=TRUE

# Disable PNG support
cmake .. -DPDFHUMMUS_NO_PNG=TRUE

# Disable TIFF support
cmake .. -DPDFHUMMUS_NO_TIFF=TRUE

# Disable all image formats
cmake .. -DPDFHUMMUS_NO_DCT=TRUE -DPDFHUMMUS_NO_PNG=TRUE -DPDFHUMMUS_NO_TIFF=TRUE
```

In FetchContent:

```cmake
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.8.0
)
set(PDFHUMMUS_NO_PNG TRUE CACHE BOOL "" FORCE)  # Disable PNG
FetchContent_MakeAvailable(PDFHummus)
```

### Disabling Encryption (PDF 2.0)

If you don't need encryption features and want to avoid OpenSSL dependency:

```bash
cmake .. -DPDFHUMMUS_NO_OPENSSL=TRUE
```

Or in CMake:

```cmake
set(PDFHUMMUS_NO_OPENSSL TRUE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(PDFHummus)
```

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
