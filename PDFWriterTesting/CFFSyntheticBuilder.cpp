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

static void AppendBigEndianOffset(string& ioCFF, size_t inValue, Byte inOffSize)
{
    for(int b = (int)inOffSize - 1; b >= 0; --b)
        ioCFF.push_back((char)((inValue >> (b * 8)) & 0xFF));
}

// Encodes a CFF DICT short-integer (operator prefix 0x1C + 16-bit big-endian
// signed value). Constant 3-byte width regardless of value, which lets the
// builder pre-compute offsets without iterating to find a stable encoding.
static void AppendShortIntDictOperand(string& ioCFF, unsigned short inValue)
{
    ioCFF.push_back('\x1C');
    ioCFF.push_back((char)((inValue >> 8) & 0xFF));
    ioCFF.push_back((char)(inValue & 0xFF));
}

string CFFSyntheticBuilder::WithCharStrings(const std::vector<std::string>& inGlyphCharStrings)
{
    const size_t glyphCountNonNotdef = inGlyphCharStrings.size();
    if(glyphCountNonNotdef + 1 > 0xFFFF) return string();

    string cff;
    cff.append(scCFFHeader, scCFFHeaderSize);

    // Name INDEX
    cff.append("\x00\x01\x01\x01\x02\x41", 6);

    // Top DICT INDEX: count=1, offSize=1, offsets=[1, 5], 4-byte body.
    // Body: /CharStrings only (no /Charset — omitting it defaults to
    // predefined ISOAdobe offset 0, which SetupSIDToGlyphMapWithStandard
    // now handles correctly).
    //   header(4) + Name(6) + TopDictIndex(9) + String(2) + GlobalSubrs(2) = 23
    const unsigned short charstringsOffset = 23;
    cff.append("\x00\x01\x01\x01\x05", 5); // count=1, offSize=1, offsets=[1, 5]
    AppendShortIntDictOperand(cff, charstringsOffset);
    cff.push_back('\x11'); // /CharStrings

    // Empty String INDEX and Global Subrs INDEX.
    // ReadStringIndex now populates mStringToSID with standard strings even
    // when mStringsCount == 0, so no dummy entry is needed.
    cff.append(scEmptyIndex, scEmptyIndexSize);
    cff.append(scEmptyIndex, scEmptyIndexSize);

    // CharStrings INDEX: count = 1 (.notdef) + inGlyphCharStrings.size().
    // Glyph 0 is a single-byte endchar; glyphs 1..N take caller bytes.
    const size_t notdefSize = 1;
    size_t totalDataLen = notdefSize;
    for(size_t i = 0; i < inGlyphCharStrings.size(); ++i)
        totalDataLen += inGlyphCharStrings[i].size();

    // Largest offset in the offset array is 1 + totalDataLen. Pick the
    // smallest offSize that fits. Refuse to silently truncate via cast.
    Byte offSize;
    if(1 + totalDataLen <= 0xFF)
        offSize = 1;
    else if(1 + totalDataLen <= 0xFFFF)
        offSize = 2;
    else
        return string(); // out of test-helper range — parser-fail by header

    const size_t glyphCount = 1 + inGlyphCharStrings.size();
    if(glyphCount > 0xFFFF)
        return string();

    cff.push_back((char)((glyphCount >> 8) & 0xFF));
    cff.push_back((char)(glyphCount & 0xFF));
    cff.push_back((char)offSize);

    // Emit (glyphCount + 1) offsets, each offSize bytes, big-endian. The
    // first offset is always 1 (offsets are 1-based, relative to the byte
    // immediately after the offsets table).
    size_t runningOffset = 1;
    AppendBigEndianOffset(cff, runningOffset, offSize);
    runningOffset += notdefSize;
    AppendBigEndianOffset(cff, runningOffset, offSize);
    for(size_t i = 0; i < inGlyphCharStrings.size(); ++i)
    {
        runningOffset += inGlyphCharStrings[i].size();
        AppendBigEndianOffset(cff, runningOffset, offSize);
    }

    // Glyph data: .notdef endchar, then caller-supplied bytes.
    cff.push_back('\x0E');
    for(size_t i = 0; i < inGlyphCharStrings.size(); ++i)
        cff.append(inGlyphCharStrings[i]);

    return cff;
}

string CFFSyntheticBuilder::WithCharset(const char* inCharsetBytes, size_t inCharsetLen)
{
    string cff;
    cff.append(scCFFHeader, scCFFHeaderSize);

    // Name INDEX
    cff.append("\x00\x01\x01\x01\x02\x41", 6);

    // Top DICT INDEX: count=1, offSize=1, offsets=[1, 5], 4-byte body.
    // Body: <short-int 23> 0x0F (/charset @ offset 23). No /CharStrings key,
    // so GetCharStringsPosition returns 0 and mCharStringsCount stays 0.
    //   header(4) + Name(6) + TopDictIndex(9) + String(2) + GlobalSubrs(2) = 23
    const unsigned short charsetOffset = 23;
    cff.append("\x00\x01\x01\x01\x05", 5);
    AppendShortIntDictOperand(cff, charsetOffset);
    cff.push_back('\x0F');

    // Empty String / Global Subrs INDEXes.
    cff.append(scEmptyIndex, scEmptyIndexSize);
    cff.append(scEmptyIndex, scEmptyIndexSize);

    cff.append(inCharsetBytes, inCharsetLen);
    return cff;
}

EStatusCode CFFSyntheticBuilder::ParseAsCFF(const string& inCFFBytes, CFFFileInput& outCFF)
{
    // data() is well-defined for empty buffers; &str[0] would be UB pre-C++11.
    InputByteArrayStream stream((Byte*)inCFFBytes.data(), (LongFilePositionType)inCFFBytes.size());
    return outCFF.ReadCFFFile(&stream);
}
