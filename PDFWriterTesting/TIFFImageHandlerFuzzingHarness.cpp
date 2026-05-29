#ifndef PDFHUMMUS_NO_TIFF

#include "PDFWriter.h"
#include "InputByteArrayStream.h"
#include "OutputStringBufferStream.h"
#include "EStatusCode.h"

extern "C" int LLVMFuzzerTestOneInput(const u_int8_t *Data, size_t Size) {
  InputByteArrayStream tiffStream((IOBasicTypes::Byte*) Data, static_cast<LongFilePositionType>( Size ));

  OutputStringBufferStream outputSink;

  PDFWriter writer;
  if (writer.StartPDFForStream(&outputSink, ePDFVersion13) != PDFHummus::eSuccess)
    return 0;

  writer.CreateFormXObjectFromTIFFStream(&tiffStream);

  writer.EndPDFForStream();

  return 0;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const u_int8_t* /*Data*/, size_t /*Size*/) {
  return 0;
}

#endif
