#include "JPEGImageParser.h"
#include "JPEGImageInformation.h"
#include "InputByteArrayStream.h"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  InputByteArrayStream stream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));

  JPEGImageParser parser;
  JPEGImageInformation info;
  parser.Parse(&stream, info);

  return 0;
}
