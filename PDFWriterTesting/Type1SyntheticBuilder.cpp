#include "Type1SyntheticBuilder.h"

#include "InputByteArrayStream.h"
#include "Type1Input.h"
#include "IOBasicTypes.h"

#include <sstream>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Type 1 stream ciphers. Same algorithm for eexec and charstrings; only the
// initial randomizer differs. See InputPFBDecodeStream.cpp / InputCharString
// DecodeStream.cpp for the decode side (this is its inverse).
static const int CIPHER_CONSTANT_1 = 52845;
static const int CIPHER_CONSTANT_2 = 22719;
static const int CIPHER_MODULU = 65536;
static const int EEXEC_RANDOMIZER_INIT = 55665;
static const int CHARSTRING_RANDOMIZER_INIT = 4330;
static const unsigned long CHARSTRING_LEN_IV = 4;
static const unsigned long EEXEC_PREFIX_BYTES = 4;

// Encrypt under Type 1's additive stream cipher. The decoder XORs incoming
// ciphertext with the high byte of the randomizer, then advances the
// randomizer using the ciphertext. So encryption mirrors that: ciphertext =
// plaintext ^ (rand >> 8); rand = (ciphertext + rand) * C1 + C2.
static string EncryptStream(const string& inPlaintext,
                            unsigned long inLeadingPadBytes,
                            int inRandomizerInit)
{
    unsigned short randomizer = (unsigned short)inRandomizerInit;
    string out;
    out.reserve(inPlaintext.size() + inLeadingPadBytes);

    // Pad bytes: encrypt arbitrary plaintext (zeros). The decoder will skip
    // these as part of lenIV / eexec preamble.
    for(unsigned long i = 0; i < inLeadingPadBytes; ++i)
    {
        Byte plain = 0x00;
        Byte cipher = (Byte)(plain ^ (randomizer >> 8));
        randomizer = (unsigned short)(((cipher + randomizer) * CIPHER_CONSTANT_1 + CIPHER_CONSTANT_2) % CIPHER_MODULU);
        out.push_back((char)cipher);
    }

    for(size_t i = 0; i < inPlaintext.size(); ++i)
    {
        Byte plain = (Byte)(unsigned char)inPlaintext[i];
        Byte cipher = (Byte)(plain ^ (randomizer >> 8));
        randomizer = (unsigned short)(((cipher + randomizer) * CIPHER_CONSTANT_1 + CIPHER_CONSTANT_2) % CIPHER_MODULU);
        out.push_back((char)cipher);
    }
    return out;
}

// PFB segment header: 0x80, type byte, then 4-byte little-endian length,
// followed by `length` bytes of data. Caller supplies one of type 1 (ascii)
// or type 2 (binary). Type 3 (EOF) is appended separately and has no length.
static string WrapPFBSegment(Byte inType, const string& inData)
{
    string out;
    out.push_back((char)0x80);
    out.push_back((char)inType);
    const unsigned long len = (unsigned long)inData.size();
    out.push_back((char)(len & 0xFF));
    out.push_back((char)((len >> 8) & 0xFF));
    out.push_back((char)((len >> 16) & 0xFF));
    out.push_back((char)((len >> 24) & 0xFF));
    out.append(inData);
    return out;
}

string Type1SyntheticBuilder::WithCharStrings(const std::vector<NamedCharString>& inGlyphs)
{
    // Canned ASCII header. The Type1Input parser only consults specific
    // tokens, so values can be minimal/synthetic. `currentfile eexec\n` is
    // the cue for the (parser-side) PFB decoder to switch to eexec-decrypted
    // reads on the next segment.
    const string asciiHeader =
        "%!PS-AdobeFont-1.0: Synth 001.000\n"
        "12 dict begin\n"
        "/FontInfo 4 dict dup begin\n"
        "/version (001.000) readonly def\n"
        "/FullName (Synth) readonly def\n"
        "/FamilyName (Synth) readonly def\n"
        "/Weight (Regular) readonly def\n"
        "end readonly def\n"
        "/FontName /Synth def\n"
        "/FontType 1 def\n"
        "/FontMatrix [0.001 0 0 0.001 0 0] readonly def\n"
        "/FontBBox {0 0 1000 1000} readonly def\n"
        "/Encoding StandardEncoding def\n"
        "/PaintType 0 def\n"
        "currentdict end\n"
        "currentfile eexec\n";

    return WithCharStrings(inGlyphs, asciiHeader);
}

string Type1SyntheticBuilder::RawPFBFromAsciiSegment(const string& inAscii)
{
    if(inAscii.size() > 0xFFFFFFFF)
        return string();
    string pfb;
    pfb.append(WrapPFBSegment(1, inAscii));
    pfb.push_back((char)0x80);
    pfb.push_back((char)3); // EOF segment
    return pfb;
}

string Type1SyntheticBuilder::WithCharStrings(const std::vector<NamedCharString>& inGlyphs,
                                              const std::string& inAsciiHeaderOverride)
{
    return WithCharStrings(inGlyphs, std::vector<std::string>(1, inAsciiHeaderOverride));
}

string Type1SyntheticBuilder::WithCharStrings(const std::vector<NamedCharString>& inGlyphs,
                                              const std::vector<std::string>& inAsciiHeaderSegments)
{

    // eexec-encrypted body. Per-charstring binary bytes are wrapped with
    // lenIV padding + charstring cipher; the resulting `<codeLength
    // bytes>` is embedded between `RD ` and ` ND` with exactly one space
    // separator on each side. The parser does Read(codeLength) raw after
    // GetNextToken("RD") which consumes exactly one trailing whitespace,
    // so any deviation in spacing here would misalign the binary section.
    ostringstream encrypted;
    encrypted << "/Private 5 dict dup begin\n";
    encrypted << "/lenIV " << CHARSTRING_LEN_IV << " def\n";

    // .notdef gets a single-byte plaintext endchar (Type 1 opcode 14).
    // After lenIV padding + charstring cipher, every charstring grows by
    // exactly CHARSTRING_LEN_IV bytes vs its plaintext size.
    typedef std::pair<std::string, std::string> NamedCharString;
    std::vector<NamedCharString> allGlyphs;
    allGlyphs.push_back(NamedCharString(".notdef", string("\x0E", 1)));
    for(size_t i = 0; i < inGlyphs.size(); ++i)
        allGlyphs.push_back(inGlyphs[i]);

    encrypted << "/CharStrings " << allGlyphs.size() << " dict dup begin\n";
    for(size_t i = 0; i < allGlyphs.size(); ++i)
    {
        const string& name = allGlyphs[i].first;
        const string& plain = allGlyphs[i].second;
        string encryptedCharString = EncryptStream(plain, CHARSTRING_LEN_IV, CHARSTRING_RANDOMIZER_INIT);

        encrypted << "/" << name << " " << encryptedCharString.size() << " RD ";
        encrypted << encryptedCharString;
        encrypted << " ND\n";
    }
    encrypted << "end\n"; // ends CharStrings inner dict
    encrypted << "end\n"; // ends Private dict
    encrypted << "put\n";

    // Wrap the encrypted body in eexec. The decoder skips 4 bytes after
    // initializing the cipher, so we prepend 4 plaintext-zero bytes that
    // become "junk" the decoder discards.
    const string encryptedBody = EncryptStream(encrypted.str(), EEXEC_PREFIX_BYTES, EEXEC_RANDOMIZER_INIT);

    // Trailing ASCII: 512 ASCII '0' chars then "cleartomark". The PFB
    // decoder skips this between binary and text segments.
    string trailer;
    for(int i = 0; i < 512; ++i) trailer.push_back('0');
    trailer.append("\ncleartomark\n");

    if(encryptedBody.size() > 0xFFFFFFFF || trailer.size() > 0xFFFFFFFF)
        return string();
    for(size_t i = 0; i < inAsciiHeaderSegments.size(); ++i)
        if(inAsciiHeaderSegments[i].size() > 0xFFFFFFFF)
            return string();

    // Each header chunk becomes its own type-1 segment. A key whose value
    // lands in the following chunk exercises GetNextToken's segment-boundary
    // "no token" path.
    string pfb;
    for(size_t i = 0; i < inAsciiHeaderSegments.size(); ++i)
        pfb.append(WrapPFBSegment(1, inAsciiHeaderSegments[i]));
    pfb.append(WrapPFBSegment(2, encryptedBody));
    pfb.append(WrapPFBSegment(1, trailer));
    // EOF segment
    pfb.push_back((char)0x80);
    pfb.push_back((char)3);
    return pfb;
}

EStatusCode Type1SyntheticBuilder::ParseAsType1(const string& inBytes, Type1Input& outType1)
{
    InputByteArrayStream stream((Byte*)inBytes.data(), (LongFilePositionType)inBytes.size());
    return outType1.ReadType1File(&stream);
}
