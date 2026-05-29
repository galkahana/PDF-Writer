// Targets whichever AES decryption stream the build actually links:
// InputAESDecodeStreamSSL (OpenSSL-backed) when USE_OPENSSL_AES is on,
// else InputAESDecodeStream (libaesgm-backed). Mirrors the same
// dispatch DecryptionHelper::CreateDecryptionReader uses, so this
// harness exercises the production AES path regardless of build flavor.

#ifdef USE_OPENSSL_AES
#include "InputAESDecodeStreamSSL.h"
typedef InputAESDecodeStreamSSL AesDecodeStream;
#else
#include "InputAESDecodeStream.h"
typedef InputAESDecodeStream AesDecodeStream;
#endif

#include "InputByteArrayStream.h"
#include "ByteList.h"
#include "IByteReader.h"

#include <cstddef>
#include <cstdint>

static const IOBasicTypes::LongBufferSizeType kReadChunk = 4096;
static const IOBasicTypes::LongBufferSizeType kMaxDecoded = 1 << 20;

// Fixed 16-byte AES-128 key. We fuzz for memory safety in the AES
// path (padding handling, partial IV read, short ciphertext) — not
// for crypto correctness, so a constant key is sufficient.
static ByteList MakeFuzzKey() {
  ByteList key;
  for (int i = 0; i < 16; ++i)
    key.push_back((IOBasicTypes::Byte) i);
  return key;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  // AesDecodeStream takes ownership of the source reader on construction
  // and deletes it on destruction — heap-allocate accordingly.
  IByteReader* raw = new InputByteArrayStream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));
  AesDecodeStream filter(raw, MakeFuzzKey());

  IOBasicTypes::Byte buffer[kReadChunk];
  IOBasicTypes::LongBufferSizeType totalRead = 0;
  while (filter.NotEnded() && totalRead < kMaxDecoded) {
    IOBasicTypes::LongBufferSizeType got = filter.Read(buffer, kReadChunk);
    if (got == 0)
      break;
    totalRead += got;
  }

  return 0;
}
