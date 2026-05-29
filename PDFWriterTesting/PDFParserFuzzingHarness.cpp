#include "PDFParser.h"
#include "PDFStreamInput.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "InputByteArrayStream.h"
#include "IByteReader.h"

#include <cstddef>
#include <cstdint>

// Caps protect the fuzzer from runaway inputs (e.g. /Count 999999999) without
// hiding the underlying parser bug — the production library still validates
// these fields on its own. We just don't want one fuzz iteration to take
// minutes when libFuzzer wants thousands of iterations per second.
static const unsigned long kMaxPages = 1000;
static const ObjectIDType kMaxObjects = 1000;
static const IOBasicTypes::LongBufferSizeType kStreamReadChunk = 4096;
static const IOBasicTypes::LongBufferSizeType kMaxStreamBytes = 1 << 20;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  InputByteArrayStream stream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));

  PDFParser parser;

  if (parser.StartPDFParsing(&stream) != PDFHummus::eSuccess)
    return 0;

  unsigned long pageCount = parser.GetPagesCount();
  if (pageCount > kMaxPages) pageCount = kMaxPages;
  for (unsigned long i = 0; i < pageCount; ++i) {
    PDFObjectCastPtr<PDFDictionary> page(parser.ParsePage(i));
    (void) page;
  }

  ObjectIDType objectCount = parser.GetObjectsCount();
  if (objectCount > kMaxObjects) objectCount = kMaxObjects;
  for (ObjectIDType id = 1; id < objectCount; ++id) {
    PDFObjectCastPtr<PDFStreamInput> streamObject(parser.ParseNewObject(id));
    if (!streamObject)
      continue;

    IByteReader* reader = parser.StartReadingFromStream(streamObject.GetPtr());
    if (!reader)
      continue;
    IOBasicTypes::Byte buffer[kStreamReadChunk];
    IOBasicTypes::LongBufferSizeType totalRead = 0;
    while (reader->NotEnded() && totalRead < kMaxStreamBytes) {
      IOBasicTypes::LongBufferSizeType got = reader->Read(buffer, kStreamReadChunk);
      if (got == 0)
        break;
      totalRead += got;
    }
    delete reader;
  }

  return 0;
}
