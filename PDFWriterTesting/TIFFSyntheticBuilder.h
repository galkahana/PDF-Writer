/*
   Source File : TIFFSyntheticBuilder.h


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


*/
#pragma once

#include <string>
#include <stdint.h>

// Test-only helper for synthesizing minimal TIFF byte streams in-process so
// regression tests for TIFFImageHandler can run without binary fixtures.
class TIFFSyntheticBuilder
{
public:
    // Single-strip uncompressed RGB TIFF with caller-chosen declared dimensions
    // and a caller-chosen declared StripByteCount. Strip payload on disk is
    // exactly inDeclaredStripByteCount bytes of zero. Use to construct sparse
    // attack shapes: huge declared decoded size vs tiny strip bytecount.
    static std::string SingleStripRGB(uint32_t inWidth,
                                      uint32_t inLength,
                                      uint32_t inDeclaredStripByteCount);
};
