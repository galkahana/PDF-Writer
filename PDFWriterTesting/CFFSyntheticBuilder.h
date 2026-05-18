#pragma once

#include "EStatusCode.h"

#include <string>
#include <vector>
#include <stddef.h>

class CFFFileInput;

// Test-only helper for synthesizing minimal CFF byte streams in-process so
// regression tests for CFFFileInput can run without binary fixtures. Each
// builder produces a complete CFF up to the point where the caller-supplied
// payload is appended; ParseAsCFF then drives ReadCFFFile against it.
//
// Pass byte literals through the CFF_BYTES macro so the length is derived from
// sizeof(literal) - 1 and embedded \x00 bytes are preserved (sizeof on a
// pointer would silently take the pointer width).
class CFFSyntheticBuilder
{
public:
    // Header (4) + caller-supplied bytes. Stops as soon as the caller's
    // bytes exhaust — useful for malformed Name INDEX cases that fail before
    // the parser reaches any later INDEX.
    static std::string HeaderPlusBytes(const char* inBytes, size_t inLen);

    // CFF up to the Global Subrs INDEX. The Top DICT body is caller-supplied
    // (capped at 254 bytes so the index offset entry stays 1-byte). Useful
    // for malformed Top-DICT operand-list tests where ReadCFFFile fails (or
    // falls back to a default) before reaching CharStrings.
    static std::string TopDictOnly(const char* inTopDictBytes, size_t inTopDictLen);

    // Non-CID CFF whose Top DICT references both /CharStrings (key 17 -> empty
    // CharStrings INDEX at offset 23) and /Encoding (key 16 -> caller payload
    // at offset 25). ReadEncoding parses the caller's bytes against
    // ReadCard8(format) and the format-0/1 readers.
    static std::string WithEncoding(const char* inEncodingBytes, size_t inEncodingLen);

    // CID CFF whose Top DICT references /ROS, /CharStrings (1-glyph INDEX at
    // offset 32), /FDArray (1-entry INDEX at offset 38), and /FDSelect (caller
    // payload at offset 43). glyphCount is 1 and fdArrayCount is 1, so the
    // OOB primitives V-030 fixed are reachable with single-byte malformed
    // fdIndex / nextRangeGlyphIndex values.
    static std::string WithFDSelect(const char* inFDSelectBytes, size_t inFDSelectLen);

    // Non-CID CFF with predefined ISOAdobe charset (offset 0) and an empty
    // String INDEX. Glyph 0 is .notdef (single-byte endchar). Glyphs 1..N take
    // their CharString bytes verbatim from inGlyphCharStrings. Under ISOAdobe
    // charset, glyph i is assigned SID i; SID i maps to Adobe Standard String
    // i, which lines up with StandardEncoding code (i + 31) for i in [1..149].
    // That lets Type 2 seac-flavored endchar operands resolve back to a chosen
    // glyph index, supporting cycle / depth tests on
    // CFFFileInput::AddDependentGlyphs.
    static std::string WithCharStrings(const std::vector<std::string>& inGlyphCharStrings);

    // Non-CID CFF whose Top DICT references /charset (key 15 -> caller payload
    // at offset 23) but omits /CharStrings, so GetCharStringsPosition returns 0
    // and mCharStringsCount stays 0. The charset format readers then run
    // against an empty CharStrings INDEX. The caller payload is the charset
    // format byte followed by any format body.
    static std::string WithCharset(const char* inCharsetBytes, size_t inCharsetLen);

    // ReadCFFFile shim wrapping the synthesized buffer in an
    // InputByteArrayStream and returning the parser's status.
    static PDFHummus::EStatusCode ParseAsCFF(const std::string& inCFFBytes, CFFFileInput& outCFF);
};

// Use only with string literals — sizeof on a pointer would silently take the
// pointer width. Reused by case tables in tests so each entry stays one line.
#define CFF_BYTES(literal) (literal), (sizeof(literal) - 1)
