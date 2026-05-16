#pragma once

#include "EStatusCode.h"

#include <string>
#include <utility>
#include <vector>
#include <stddef.h>

class Type1Input;

// Test-only helper for synthesizing minimal Type 1 PFB byte streams in-process
// so regression tests for Type1Input can run without binary fixtures. The PFB
// is built around two cipher layers (eexec around the Private section, then
// CharString cipher around each per-glyph payload) and three PFB segments.
//
// Glyph names from inGlyphs land in /CharStrings verbatim. Each plaintext
// byte sequence is wrapped with lenIV (4) leading bytes of padding then
// charstring-encrypted, so the resulting `charString.Code` round-trips back
// to the caller's plaintext when Type1Input later runs the interpreter.
//
// Glyph 0 (.notdef) is auto-included with a single-byte endchar plaintext.
class Type1SyntheticBuilder
{
public:
    // Each pair = (glyph name, plaintext charstring bytes — pre-encryption).
    // Plaintext should already follow Type 1 charstring encoding (operand
    // bytes + operator bytes, no lenIV padding — the builder adds it).
    typedef std::pair<std::string, std::string> NamedCharString;

     // Build the full PFB. Returns empty string on failure (e.g. segment
     // sizes that don't fit in the PFB segment-length field).
    static std::string WithCharStrings(const std::vector<NamedCharString>& inGlyphs);

     // Same, but supply the ASCII (font dictionary) segment verbatim instead
     // of the canned one. The override must still end so the parser flows
     // into the eexec body (the canned header ends with
     // "currentdict end\ncurrentfile eexec\n"). Used to parse a valid font
     // that deliberately omits keys.
    static std::string WithCharStrings(const std::vector<NamedCharString>& inGlyphs,
                                       const std::string& inAsciiHeaderOverride);

     // As above, but the ASCII header is emitted as one type-1 PFB segment
     // per vector element (in order). A key at the end of one segment whose
     // value is in the next exercises GetNextToken's segment-boundary
     // "no token" return — the case the looping ReadNextTokenValue must
     // tolerate without rejecting a valid font.
    static std::string WithCharStrings(const std::vector<NamedCharString>& inGlyphs,
                                       const std::vector<std::string>& inAsciiHeaderSegments);

     // A PFB of a single ASCII segment carrying inAscii, then the EOF segment
     // — nothing after it. GetNextToken returns no token (first = false) once
     // inAscii is exhausted, so this is the minimal way to feed a dictionary
     // key whose value token is genuinely absent.
    static std::string RawPFBFromAsciiSegment(const std::string& inAscii);

    // ReadType1File shim. After this returns, the parser owns its data
    // (charstrings are copied into heap buffers, no stream-lifetime
    // dependency), so the caller may discard inBytes immediately.
    static PDFHummus::EStatusCode ParseAsType1(const std::string& inBytes, Type1Input& outType1);
};
