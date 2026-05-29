#include "OpenTypeFileInput.h"
#include "InputByteArrayStream.h"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  InputByteArrayStream stream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));

  OpenTypeFileInput parser;
  parser.ReadOpenTypeFile(&stream, 0);

  return 0;
}
