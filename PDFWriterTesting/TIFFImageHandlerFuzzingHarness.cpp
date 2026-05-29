#include "PDFWriter.h"
#include "PDFFormXObject.h"
#include "InputByteArrayStream.h"
#include "OutputStringBufferStream.h"
#include "EStatusCode.h"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  InputByteArrayStream tiffStream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));

  OutputStringBufferStream outputSink;

  PDFWriter writer;
  if (writer.StartPDFForStream(&outputSink, ePDFVersion13) != PDFHummus::eSuccess)
    return 0;

  PDFFormXObject* formXObject = writer.CreateFormXObjectFromTIFFStream(&tiffStream);
  delete formXObject;

  writer.EndPDFForStream();

  return 0;
}
