/*
   Source File : TIFFSyntheticBuilder.cpp


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
#include "TIFFSyntheticBuilder.h"

#include <string>
#include <stdint.h>

using std::string;

static void AppendU16LE(string& ioOut, uint16_t inValue)
{
    ioOut.push_back(static_cast<char>(inValue & 0xFF));
    ioOut.push_back(static_cast<char>((inValue >> 8) & 0xFF));
}

static void AppendU32LE(string& ioOut, uint32_t inValue)
{
    ioOut.push_back(static_cast<char>(inValue & 0xFF));
    ioOut.push_back(static_cast<char>((inValue >> 8) & 0xFF));
    ioOut.push_back(static_cast<char>((inValue >> 16) & 0xFF));
    ioOut.push_back(static_cast<char>((inValue >> 24) & 0xFF));
}

// TIFF type codes (TIFF 6.0 §2)
static const uint16_t kTypeShort    = 3;
static const uint16_t kTypeLong     = 4;
static const uint16_t kTypeRational = 5;

// TIFF tag codes
static const uint16_t kTagImageWidth                = 256;
static const uint16_t kTagImageLength               = 257;
static const uint16_t kTagBitsPerSample             = 258;
static const uint16_t kTagCompression               = 259;
static const uint16_t kTagPhotometricInterpretation = 262;
static const uint16_t kTagStripOffsets              = 273;
static const uint16_t kTagSamplesPerPixel           = 277;
static const uint16_t kTagRowsPerStrip              = 278;
static const uint16_t kTagStripByteCounts           = 279;
static const uint16_t kTagXResolution               = 282;
static const uint16_t kTagYResolution               = 283;
static const uint16_t kTagResolutionUnit            = 296;

static void AppendIFDEntry(string& ioOut,
                           uint16_t inTag,
                           uint16_t inType,
                           uint32_t inCount,
                           uint32_t inValueOrOffset)
{
    AppendU16LE(ioOut, inTag);
    AppendU16LE(ioOut, inType);
    AppendU32LE(ioOut, inCount);
    AppendU32LE(ioOut, inValueOrOffset);
}

string TIFFSyntheticBuilder::SingleStripRGB(uint32_t inWidth,
                                            uint32_t inLength,
                                            uint32_t inDeclaredStripByteCount)
{
    // Layout (little-endian, 12-entry IFD):
    //   [0..7]       header
    //   [8..157]     IFD (2 + 12*12 + 4 = 150 bytes)
    //   [158..163]   BitsPerSample external data (3 SHORTs)
    //   [164..171]   XResolution external data (RATIONAL = 2 LONGs)
    //   [172..179]   YResolution external data (RATIONAL = 2 LONGs)
    //   [180..]      strip payload
    const uint32_t kIFDOffset             = 8;
    const uint32_t kBitsPerSampleDataOff  = 158;
    const uint32_t kXResolutionDataOff    = 164;
    const uint32_t kYResolutionDataOff    = 172;
    const uint32_t kStripDataOff          = 180;

    string out;
    out.reserve(kStripDataOff + inDeclaredStripByteCount);

    // Header
    out.push_back('I');
    out.push_back('I');
    AppendU16LE(out, 42);
    AppendU32LE(out, kIFDOffset);

    // IFD entry count
    AppendU16LE(out, 12);

    // Entries (must be sorted ascending by tag)
    AppendIFDEntry(out, kTagImageWidth,                kTypeLong,     1, inWidth);
    AppendIFDEntry(out, kTagImageLength,               kTypeLong,     1, inLength);
    AppendIFDEntry(out, kTagBitsPerSample,             kTypeShort,    3, kBitsPerSampleDataOff);
    AppendIFDEntry(out, kTagCompression,               kTypeShort,    1, 1);   // none
    AppendIFDEntry(out, kTagPhotometricInterpretation, kTypeShort,    1, 2);   // RGB
    AppendIFDEntry(out, kTagStripOffsets,              kTypeLong,     1, kStripDataOff);
    AppendIFDEntry(out, kTagSamplesPerPixel,           kTypeShort,    1, 3);
    AppendIFDEntry(out, kTagRowsPerStrip,              kTypeLong,     1, inLength);
    AppendIFDEntry(out, kTagStripByteCounts,           kTypeLong,     1, inDeclaredStripByteCount);
    AppendIFDEntry(out, kTagXResolution,               kTypeRational, 1, kXResolutionDataOff);
    AppendIFDEntry(out, kTagYResolution,               kTypeRational, 1, kYResolutionDataOff);
    AppendIFDEntry(out, kTagResolutionUnit,            kTypeShort,    1, 2);   // inch

    // Next IFD offset (none)
    AppendU32LE(out, 0);

    // External data: BitsPerSample {8, 8, 8}
    AppendU16LE(out, 8);
    AppendU16LE(out, 8);
    AppendU16LE(out, 8);

    // External data: XResolution = 72/1
    AppendU32LE(out, 72);
    AppendU32LE(out, 1);

    // External data: YResolution = 72/1
    AppendU32LE(out, 72);
    AppendU32LE(out, 1);

    // Strip payload: exactly inDeclaredStripByteCount zeros
    out.append(inDeclaredStripByteCount, '\0');

    return out;
}
