#include "InputFlateDecodeStream.h"
#include "InputByteArrayStream.h"

#include <cstddef>
#include <cstdint>

// Bound the decoded-output read so zip-bomb shaped inputs don't stall one
// fuzz iteration for minutes — production callers validate stream length
// themselves. This cap is fuzzer throughput tuning, not a security bound.
static const IOBasicTypes::LongBufferSizeType kReadChunk = 4096;
static const IOBasicTypes::LongBufferSizeType kMaxDecoded = 1 << 20;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  // InputFlateDecodeStream takes ownership of the source reader on construction
  // and deletes it on destruction — heap-allocate accordingly.
  IByteReader* raw = new InputByteArrayStream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));
  InputFlateDecodeStream filter(raw);

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
