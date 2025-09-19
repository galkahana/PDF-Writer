# Claude Code Context - OpenSSL AES Migration

## Project Goal
Replace LibAesgm AES encryption with OpenSSL-based alternatives while maintaining backward compatibility. This will eventually allow removing the LibAesgm dependency entirely.

## Current AES Usage Analysis

### 1. Stream Classes (PDF 1.4+ encryption)
- `InputAESDecodeStream` - AES CBC decryption for PDF streams
- `OutputAESEncodeStream` - AES CBC encryption for PDF streams
- Located in: `PDFWriter/Input|OutputAESEncodeStream.{h,cpp}`
- Used by: `EncryptionHelper.cpp`, `DecryptionHelper.cpp`

### 2. XCryptionCommon2_0 (PDF 2.0 encryption)
- Functions: `encryptKeyCBC()`, `encryptKeyECB()`, `decryptKeyCBC()`, `decryptKeyECB()`
- Located in: `PDFWriter/XCryptionCommon2_0.cpp`
- Uses both OpenSSL (SHA-256/384/512) and LibAesgm (AES operations)

## Coding Standards Discovered
- **C99 Compatible**: No modern C++ features, conservative early 2000s style
- **Hungarian Notation**: `m` prefix for members, `in` prefix for parameters
- **Manual Memory Management**: `new[]`/`delete[]`, no smart pointers
- **Interface Naming**: `I` prefix (e.g., `IByteReader`)
- **ByteList**: typedef for `std::basic_string<IOBasicTypes::Byte>`
- **CamelCase**: Classes and methods

## Implementation Plan

### Phase 1: CMake Configuration ✅
- Add `USE_OPENSSL_AES` option (default OFF for compatibility) ✅
- Conditional compilation: `#ifdef USE_OPENSSL_AES` ✅
- Skip LibAesgm linking when using OpenSSL AES ✅
- Early OpenSSL detection and `USE_OPENSSL_AES` determination ✅

### Phase 2: OpenSSL Stream Classes ✅
- `InputAESDecodeStreamSSL` - EVP-based AES CBC decryption ✅
- `OutputAESEncodeStreamSSL` - EVP-based AES CBC encryption ✅
- Match existing API exactly, same constructor signatures ✅
- Added `AESConstants.h` for shared constants ✅

### Phase 3: XCryptionCommon2_0 Alternatives ✅
- OpenSSL versions of: `encryptKeyCBC()`, `encryptKeyECB()` ✅
- Shared decrypt functions using LibAesgm implementation ✅
- Refactored with do-while-false error handling pattern ✅
- Conditional compilation throughout the file ✅

### Phase 4: Factory Pattern Updates ✅
- Update `EncryptionHelper.cpp` and `DecryptionHelper.cpp` ✅
- Choose implementation based on `USE_OPENSSL_AES` flag ✅
- Maintain identical external API ✅

## Build Commands
```bash
# Build with LibAesgm (default)
cmake ..
cmake --build . --config Release

# Build with OpenSSL AES
cmake .. -DUSE_OPENSSL_AES=ON
cmake --build . --config Release

# Test
ctest --test-dir . -C Release
```

## Implementation Notes & Lessons Learned

### CMake Issues Fixed
- Fixed CMake bug: `PDFHUMMUS_DEPENDS_OPENSSL` was incorrectly set to `find_dependency(PNG)` instead of `find_dependency(OpenSSL)`
- Consolidated duplicate OpenSSL detection logic between `USE_BUNDLED=TRUE/FALSE` sections
- LibAesgm linking now properly skipped when `USE_OPENSSL_AES=TRUE`

### Technical Implementation Details
- OpenSSL EVP API requires `EVP_CIPHER_CTX_set_padding(ctx, 0)` to match LibAesgm's manual padding behavior
- XCryptionCommon2_0 uses both CBC (with zero IV) and ECB modes for PDF 2.0 key derivation functions
- All AES operations needed OpenSSL alternatives, not just the stream classes:
  - `encryptKeyCBC()`, `encryptKeyECB()`, `decryptKeyCBCZeroIV()`, `decryptKeyECB()`
- AES block size hardcoded as `16` in OpenSSL version instead of `AES_BLOCK_SIZE` for consistency

### Code Style Observations
- Codebase maintains C99 compatibility - no modern C++ features used
- Hungarian notation: `m` prefix for members, `in` prefix for parameters
- Manual memory management throughout (`new[]`/`delete[]`, no smart pointers)
- Conservative early 2000s C++ style maintained for consistency

### Build System Logic
- `USE_OPENSSL_AES` automatically determined: `TRUE` if OpenSSL available and `PDFHUMMUS_NO_OPENSSL=FALSE`
- Conditional source file inclusion working via CMake variables
- Factory pattern updates in both `EncryptionHelper.cpp` and `DecryptionHelper.cpp`

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
- **Resource management**: Always clean up dynamically allocated memory and OpenSSL contexts

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

## Current Status
- ✅ Implementation complete
- ✅ Build system verified
- ✅ Code review guidelines documented
- ✅ Ready for testing