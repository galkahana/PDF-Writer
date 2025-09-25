# Claude Code Context - PDF-Writer Development Guide

## Project Overview
PDF-Writer is a C++ library for creating and manipulating PDF files. This guide contains development patterns, coding standards, and best practices discovered during active development.

## Coding Standards Discovered
- **C99 Compatible**: No modern C++ features, conservative early 2000s style
- **Hungarian Notation**: `m` prefix for members, `in` prefix for parameters
- **Manual Memory Management**: `new[]`/`delete[]`, no smart pointers
- **Interface Naming**: `I` prefix (e.g., `IByteReader`)
- **ByteList**: typedef for `std::basic_string<IOBasicTypes::Byte>`
- **CamelCase**: Classes and methods

## Project Structure

### Key Components
- **PDFWriter**: Core library for PDF creation and manipulation
- **PDFWriterTesting**: Comprehensive test suite with 82+ tests
- **Conditional Features**: Support for optional dependencies (OpenSSL, LibJpeg, LibPng, LibTiff)
- **Cross-platform**: Works on multiple operating systems and architectures

## Build Commands
```bash
# Standard build
cmake ..
cmake --build . --config Release

# Build with specific features disabled
cmake .. -DPDFHUMMUS_NO_OPENSSL=TRUE    # Disable OpenSSL/PDF 2.0 support
cmake .. -DPDFHUMMUS_NO_PNG=TRUE        # Disable PNG support
cmake .. -DPDFHUMMUS_NO_DCT=TRUE        # Disable JPEG support
cmake .. -DPDFHUMMUS_NO_TIFF=TRUE       # Disable TIFF support

# Run tests
ctest --test-dir . -C Release
```

## Development Notes & Patterns

### Community Solution Patterns (PR #313)
Based on successful outline/annotation example implementation:

#### Helper Class Approach
- **OutlineBuilder Pattern**: Create dedicated helper classes for complex PDF operations
- **Flexible APIs**: Support both simple defaults and advanced customization
- **Recursive Generation**: Use recursive methods for hierarchical PDF structures
- **Example**: `OutlineBuilder` class with `CreateOutlineTree()` and `WriteOutlineItems()`

#### Document Extension Pattern
- **IDocumentContextExtender**: Use for modifying PDF catalog and document structure
- **DocumentContextExtenderAdapter**: Extend this base class and override specific methods
- **OnCatalogWrite()**: Key method for attaching document-level structures like outlines
- **Integration**: Register extenders via `GetDocumentContext().AddDocumentContextExtender()`

#### API Design Principles Proven Effective
- **"Make common cases simple"**: Provide convenient defaults for typical usage
- **Support exceptions**: Allow per-item overrides and edge case handling
- **Overloaded constructors**: Multiple ways to create objects based on complexity needed
- **Optional parameters**: Use optional params for advanced features, required for basics

#### Implementation Details
- **UTF-8 Support**: Use `PDFTextString` for international text in outlines
- **Page References**: Get page IDs via `WritePageAndReturnPageID()` for destinations
- **Object Management**: Use `ObjectsContext` for creating PDF indirect objects
- **Resource Cleanup**: Follow do-while-false pattern with proper cleanup

#### Helper Structure Examples
```cpp
struct OutlineItem {
    std::string title;
    ObjectIDType pageReference;
    std::vector<OutlineItem> children;  // Support nesting
    // Optional customization fields
};
```

### Conditional Compilation
- Tests that require specific dependencies should be wrapped with appropriate `#ifndef` guards
- Examples: `#ifndef PDFHUMMUS_NO_OPENSSL`, `#ifndef PDFHUMMUS_NO_PNG`
- Provide fallback implementations that return success (0) when features are disabled
- Follow patterns established in `JPGImageTest.cpp`, `PDFWithPassword.cpp`, etc.

## Code Review & Development Guidelines

### Change Management Best Practices
- **Minimal, surgical changes**: Only modify what's necessary for the feature
- **Avoid cosmetic changes**: Don't fix formatting/spacing that makes diffs noisier and harder to review
- **Preserve original locations**: Keep functions in their original positions when possible to minimize review complexity
- **Explain vs. edit**: Prefer explaining needed changes rather than making direct edits (especially for AI assistance)

### Error Handling Patterns
- **Do-while-false pattern**: Use for functions with multiple cleanup steps and potential early exits
- **Early returns**: Handle initial validation failures immediately to simplify main logic flow
- **Single cleanup section**: Consolidate all resource cleanup in one place at function end
- **Resource management**: Always clean up dynamically allocated memory and external library contexts (e.g., OpenSSL, FreeType)

### Code Structure Best Practices
- **Factor out duplication**: When you see repeated patterns, especially error-prone ones, create shared helper functions
- **Specific parameters**: Pass specific objects (like cipher types) rather than boolean flags to eliminate conditionals
- **Proper conditional compilation**: Ensure `#ifdef`/`#else`/`#endif` blocks are correctly structured and placed

### Separation of Concerns
- **Domain-specific constants**: Keep specialized constants separate from general-purpose utilities (e.g., AES constants vs IO types)
- **Use `#define` for constants**: Prefer preprocessor defines over namespace constants for library-wide values
- **Dedicated header files**: Create focused headers for specific domains rather than adding to general-purpose files

### Existing Codebase Patterns to Follow
- **Hungarian notation**: `m` prefix for members, `in` prefix for parameters
- **Conservative C++ style**: Prioritize clarity and reliability over modern language features
- **Manual memory management**: Use `new[]`/`delete[]` consistently with the existing codebase
- **Interface naming**: `I` prefix for interfaces (e.g., `IByteReader`)

## Testing Guidelines

### Test Organization
- Tests are located in `PDFWriterTesting/` directory
- Each test should be self-contained and return 0 for success, non-zero for failure
- Use `BuildRelativeOutputPath()` and `BuildRelativeInputPath()` for file paths
- Test materials should be placed in `PDFWriterTesting/Materials/`

### Running Tests
- Full test suite: `ctest --test-dir . -C Release`
- Specific test: `ctest --test-dir . -C Release -R TestName`
- Verbose output: `ctest --test-dir . -C Release --verbose -R TestName`
- Output on failure: `ctest --test-dir . -C Release --output-on-failure`