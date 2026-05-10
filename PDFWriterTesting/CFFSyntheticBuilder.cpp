#include "CFFSyntheticBuilder.h"

#include "CFFFileInput.h"
#include "InputByteArrayStream.h"
#include "IOBasicTypes.h"

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Header: major=1, minor=0, hdrSize=4, absOffSize=1.
static const char scCFFHeader[] = "\x01\x00\x04\x01";
static const size_t scCFFHeaderSize = sizeof(scCFFHeader) - 1;

// Empty INDEX (count=0). Used as filler for String / Global Subrs INDEXes and,
// in the encoding builder, the CharStrings INDEX as well.
static const char scEmptyIndex[] = "\x00\x00";
static const size_t scEmptyIndexSize = sizeof(scEmptyIndex) - 1;

string CFFSyntheticBuilder::HeaderPlusBytes(const char* inBytes, size_t inLen)
{
    string cff;
    cff.append(scCFFHeader, scCFFHeaderSize);
    cff.append(inBytes, inLen);
    return cff;
}

string CFFSyntheticBuilder::TopDictOnly(const char* inTopDictBytes, size_t inTopDictLen)
{
    string cff;
    // Top DICT length is capped at 254 so the second offset of the single-
    // entry INDEX stays 1-byte. Refuse to silently truncate via (Byte)cast —
    // an inconsistent INDEX would surface as a confusing parse failure
    // unrelated to whatever the caller was trying to test. Returning empty
    // makes ReadCFFFile fail on the header check.
    if(inTopDictLen > 254)
        return cff;

    cff.append(scCFFHeader, scCFFHeaderSize);

    // Name INDEX: count=1, offSize=1, offsets=[1, 2], data="A"
    cff.append("\x00\x01\x01\x01\x02\x41", 6);

    // Top DICT INDEX: count=1, offSize=1, offsets=[1, 1+len], data=<top dict>.
    cff.append("\x00\x01\x01\x01", 4);
    cff.push_back((char)(1 + (Byte)inTopDictLen));
    cff.append(inTopDictBytes, inTopDictLen);

    // String INDEX (empty), Global Subrs INDEX (empty)
    cff.append(scEmptyIndex, scEmptyIndexSize);
    cff.append(scEmptyIndex, scEmptyIndexSize);

    return cff;
}

string CFFSyntheticBuilder::WithEncoding(const char* inEncodingBytes, size_t inEncodingLen)
{
    string cff;
    cff.append(scCFFHeader, scCFFHeaderSize);

    // Name INDEX
    cff.append("\x00\x01\x01\x01\x02\x41", 6);

    // Top DICT INDEX: count=1, offSize=1, offsets=[1, 5], 4-byte top dict.
    // Top dict: <int 23> 0x11 (CharStrings), <int 25> 0x10 (Encoding).
    // Single-byte CFF int encodes value-139, so 23->0xA2, 25->0xA4.
    cff.append("\x00\x01\x01\x01\x05" "\xA2\x11\xA4\x10", 9);

    // Empty String / Global Subrs / CharStrings INDEXes (each: count=0).
    cff.append(scEmptyIndex, scEmptyIndexSize);
    cff.append(scEmptyIndex, scEmptyIndexSize);
    cff.append(scEmptyIndex, scEmptyIndexSize);

    cff.append(inEncodingBytes, inEncodingLen);
    return cff;
}

string CFFSyntheticBuilder::WithFDSelect(const char* inFDSelectBytes, size_t inFDSelectLen)
{
    string cff;
    cff.append(scCFFHeader, scCFFHeaderSize);

    // Name INDEX
    cff.append("\x00\x01\x01\x01\x02\x41", 6);

    // Top DICT INDEX: count=1, offSize=1, offsets=[1, 14], 13-byte top dict.
    //   <int 32> 0x11           CharStrings @ 32 -> 0xAB
    //   <int 38> 0x0C 0x24      FDArray     @ 38 -> 0xB1
    //   <int 43> 0x0C 0x25      FDSelect    @ 43 -> 0xB6
    //   0x8C 0x8C 0x8B 0x0C 0x1E   /ROS with SID 1, SID 1, supplement 0
    cff.append("\x00\x01\x01\x01\x0E"
               "\xAB\x11"
               "\xB1\x0C\x24"
               "\xB6\x0C\x25"
               "\x8C\x8C\x8B\x0C\x1E", 18);

    // Empty String / Global Subrs INDEXes
    cff.append(scEmptyIndex, scEmptyIndexSize);
    cff.append(scEmptyIndex, scEmptyIndexSize);

    // CharStrings INDEX: count=1, offSize=1, offsets=[1, 2], data=0x0E (endchar)
    cff.append("\x00\x01\x01\x01\x02\x0E", 6);

    // FDArray INDEX: count=1, offSize=1, offsets=[1, 1] (single zero-length
    // font dict — ReadDict returns empty dict, ReadPrivateDict treats missing
    // /Private as start=end=0).
    cff.append("\x00\x01\x01\x01\x01", 5);

    cff.append(inFDSelectBytes, inFDSelectLen);
    return cff;
}

EStatusCode CFFSyntheticBuilder::ParseAsCFF(const string& inCFFBytes, CFFFileInput& outCFF)
{
    // data() is well-defined for empty buffers; &str[0] would be UB pre-C++11.
    InputByteArrayStream stream((Byte*)inCFFBytes.data(), (LongFilePositionType)inCFFBytes.size());
    return outCFF.ReadCFFFile(&stream);
}
