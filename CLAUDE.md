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

### Phase 1: CMake Configuration
- Add `USE_OPENSSL_AES` option (default OFF for compatibility)
- Conditional compilation: `#ifdef USE_OPENSSL_AES`
- Skip LibAesgm linking when using OpenSSL AES

### Phase 2: OpenSSL Stream Classes
- `InputAESDecodeStreamSSL` - EVP-based AES CBC decryption
- `OutputAESEncodeStreamSSL` - EVP-based AES CBC encryption
- Match existing API exactly, same constructor signatures

### Phase 3: XCryptionCommon2_0 Alternatives
- OpenSSL versions of: `encryptKeyCBC()`, `encryptKeyECB()`, etc.
- Static functions with same signatures
- Conditional compilation throughout the file

### Phase 4: Factory Pattern Updates
- Update `EncryptionHelper.cpp` and `DecryptionHelper.cpp`
- Choose implementation based on `USE_OPENSSL_AES` flag
- Maintain identical external API

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

## Current Status
- ✅ Analysis complete
- 🔄 Implementation in progress
- ⏳ Testing pending