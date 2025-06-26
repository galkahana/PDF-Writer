# PDF-Writer (PDFHummus)

A fast and free C++ library for creating, parsing and manipulating PDF files and streams.

📖 [Documentation](https://github.com/galkahana/PDF-Writer/wiki) | 🌐 [Project Site](http://www.pdfhummus.com) | 🟢 [NodeJS Wrapper](https://github.com/julianhille/MuhammaraJS)

## Prerequisites

- **Compiler**: Visual Studio (Windows), GCC/Clang (Linux/macOS)
- **CMake**: [Download here](https://cmake.org/)
- **OpenSSL**: Required for PDF2.0 encryption (set `PDFHUMMUS_NO_OPENSSL=TRUE` to disable)

## Project Structure

- **PDFWriter**: Main library implementation
- **PDFWriterTesting**: Test code for ctest
- **FreeType, LibAesgm, LibJpeg, LibPng, LibTiff, Zlib**: Bundled dependencies

## Quick Start

```bash
# Build
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Test
ctest --test-dir . -C Release

# Install
cmake --install . --prefix ./install --config Release
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `PDFHUMMUS_NO_DCT` | FALSE | Exclude DCT decoding support/Detach LibJpeg dependency |
| `PDFHUMMUS_NO_TIFF` | FALSE | Exclude TIFF Images support/Detach LibTiff dependency |
| `PDFHUMMUS_NO_PNG` | FALSE | Exclude PNG Images support/Detatch LibgPng dependency |
| `PDFHUMMUS_NO_OPENSSL` | FALSE | Exclude PDF2.0 encryption/Detach OpenSSL dependency |
| `USE_BUNDLED` | TRUE | Use bundled dependencies |
| `USE_UNBUNDLED_FALLBACK_BUNDLED` | FALSE | Fallback to bundled if system libs not found |
| `BUILD_FUZZING_HARNESS` | FALSE | Enable fuzz testing |

Example: `cmake .. -DUSE_BUNDLED=FALSE`

## Using in Your Project

### Option 1: CMake FetchContent (Recommended)
```cmake
include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.6.2
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)
target_link_libraries(YourTarget PDFHummus::PDFWriter)
```

### Option 2: find_package
```cmake
find_package(PDFHummus REQUIRED)
target_link_libraries(YourTarget PDFHummus::PDFWriter)
```

### Option 3: Copy Sources
Simply copy the source folders to your project and include them directly.

## Development

### Testing
```bash
# Run tests
ctest --test-dir build -C Release -j8

# Build and test 
cmake --build build --target pdfWriterCheck --config Release
```
Test output files: `./build/Testing/Output`

### VS Code Setup
Install these extensions:
- C/C++ Extension Pack  
- CMake Tools
- CMake Test Explorer

### Packaging
```bash
cd build && cpack .
```

## Additional Build Instructions

**iOS**: See [build guide](http://pdfhummus.com/post/45501609236/how-to-build-iphone-apps-that-use-pdfhummus)

**Manual builds**: All PDFWriter sources are in the `PDFWriter` folder. Link against Zlib, LibTiff, LibJpeg, LibPng, FreeType, and OpenSSL.
