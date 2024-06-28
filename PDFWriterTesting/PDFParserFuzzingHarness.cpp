#include "PDFParser.h"
#include "InputByteArrayStream.h"

extern "C" int LLVMFuzzerTestOneInput(const u_int8_t *Data, size_t Size) {
  InputByteArrayStream stream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));

  PDFParser parser;

  parser.StartPDFParsing(&stream);

  return 0;
}