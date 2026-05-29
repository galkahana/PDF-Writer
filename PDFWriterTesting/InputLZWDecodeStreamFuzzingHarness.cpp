#include "InputLZWDecodeStream.h"
#include "InputByteArrayStream.h"

#include <cstddef>
#include <cstdint>

static const IOBasicTypes::LongBufferSizeType kReadChunk = 4096;
static const IOBasicTypes::LongBufferSizeType kMaxDecoded = 1 << 20;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  // InputLZWDecodeStream takes ownership of the source reader on construction
  // and deletes it on destruction — heap-allocate accordingly.
  IByteReader* raw = new InputByteArrayStream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));
  InputLZWDecodeStream filter(raw);

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
