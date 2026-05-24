/*
   Source File : TIFFImageHandlerTest.cpp


   Copyright 2026 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


   Unit-style regression tests for TIFFImageHandler against synthetic byte
   streams. Broader integration coverage with real TIFFs lives in
   TIFFImageTest.cpp.

*/

#ifndef PDFHUMMUS_NO_TIFF

#include "PDFWriter.h"
#include "PDFFormXObject.h"
#include "InputByteArrayStream.h"
#include "OutputStringBufferStream.h"
#include "EStatusCode.h"

#include "TIFFSyntheticBuilder.h"

#include <iostream>
#include <string>
#include <stdint.h>

using namespace std;
using namespace PDFHummus;

// ---- CalculateTiffSizeNoTiles cases -----------------------------------------

struct CalculateTiffSizeNoTilesCase {
    const char* label;
    uint32_t width;
    uint32_t length;
    uint32_t declaredStripByteCount;
    bool expectRejection;
};

// Cases for the declared-decoded vs strip-bytecount ratio guard.
static const CalculateTiffSizeNoTilesCase scCalculateTiffSizeNoTilesCases[] = {
    // 50000 x 50000 RGB = 7.5 GB decoded vs 5 KB strip → 1.5M:1 ratio, above K=10000.
    {"SparsePayloadRatioExceedsCap_Rejects", 50000, 50000, 5000, true},
    // 10 x 10 RGB uncompressed = 300 bytes decoded == 300 bytes strip → 1:1 ratio.
    {"BalancedRatio_Accepts",                10,    10,    300,  false},
};

static bool RunCalculateTiffSizeNoTilesCases()
{
    const size_t count =
        sizeof(scCalculateTiffSizeNoTilesCases) /
        sizeof(scCalculateTiffSizeNoTilesCases[0]);

    for(size_t i = 0; i < count; ++i) {
        const CalculateTiffSizeNoTilesCase& c = scCalculateTiffSizeNoTilesCases[i];

        // Arrange — synthesize a minimal TIFF with caller-chosen declared
        // dimensions and declared strip bytecount. Output sinks to a memory
        // buffer so the test produces no on-disk artifact.
        PDFWriter writer;
        OutputStringBufferStream outputSink;
        if(writer.StartPDFForStream(&outputSink, ePDFVersion13) != eSuccess) {
            cout << "TIFFImageHandlerTest [CalculateTiffSizeNoTiles::"
                 << c.label << "]: StartPDFForStream failed" << endl;
            return false;
        }
        string tiff = TIFFSyntheticBuilder::SingleStripRGB(
            c.width, c.length, c.declaredStripByteCount);
        InputByteArrayStream stream(
            reinterpret_cast<IOBasicTypes::Byte*>(&tiff[0]),
            static_cast<LongFilePositionType>(tiff.size()));

        // Act
        PDFFormXObject* formXObject =
            writer.CreateFormXObjectFromTIFFStream(&stream);
        writer.EndPDFForStream(); // best-effort cleanup; assertion is on the handler

        // Assert — handler returns NULL exactly when the ratio guard fires.
        bool rejected = (formXObject == NULL);
        delete formXObject;
        if(rejected != c.expectRejection) {
            cout << "TIFFImageHandlerTest [CalculateTiffSizeNoTiles::"
                 << c.label << "]: expected rejection=" << c.expectRejection
                 << " but got rejected=" << rejected << endl;
            return false;
        }
    }
    return true;
}

// ---- Entry point ------------------------------------------------------------

int TIFFImageHandlerTest(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    if(!RunCalculateTiffSizeNoTilesCases()) return 1;
    return 0;
}

#else

int TIFFImageHandlerTest(int argc, char* argv[]) { return 0; }

#endif
