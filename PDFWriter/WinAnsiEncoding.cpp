#include "WinAnsiEncoding.h"
#include "BetweenIncluding.h"

static const char* scGlyphNames[256] = 
{
		".notdef", ".notdef", ".notdef", ".notdef",".notdef", ".notdef", ".notdef", ".notdef",
		".notdef", ".notdef", ".notdef", ".notdef",".notdef", ".notdef", ".notdef", ".notdef",
		".notdef", ".notdef", ".notdef", ".notdef",".notdef", ".notdef", ".notdef", ".notdef",
		".notdef", ".notdef", ".notdef", ".notdef",".notdef", ".notdef", ".notdef", ".notdef",
		"space", "exclam", "quotedbl", "numbersign","dollar", "percent", "ampersand", "quotesingle",
		"parenleft", "parenright", "asterisk", "plus","comma", "hyphen", "period", "slash",
		"zero", "one", "two", "three","four", "five", "six", "seven",
		"eight", "nine", "colon", "semicolon","less", "equal", "greater", "question",
		"at", "A", "B", "C","D", "E", "F", "G",
		"H", "I", "J", "K","L", "M", "N", "O",
		"P", "Q", "R", "S","T", "U", "V", "W",
		"X", "Y", "Z", "bracketleft","backslash", "bracketright", "asciicircum", "underscore",
		"grave", "a", "b", "c","d", "e", "f", "g",
		"h", "i", "j", "k","l", "m", "n", "o",
		"p", "q", "r", "s","t", "u", "v", "w",
		"x", "y", "z", "braceleft","bar", "braceright", "asciitilde", ".notdef",
		"Euro", ".notdef", "quotesinglbase", ".notdef","quotedblbase", "ellipsis", "dagger", "daggerdbl",
		"circumflex", "perthousand", "Scaron", "guilsinglleft","OE", ".notdef", "quoteright", ".notdef",
		".notdef", "quoteleft", ".notdef", "quotedblleft","quotedblright", "bullet", "endash", "emdash",
		"tilde", "trademark", "scaron", "guilsinglright","oe", ".notdef", "zcaron", "Ydieresis",
		".notdef", "exclamdown", "cent", "sterling","currency", "yen", "brokenbar", "section",
		"dieresis", "copyright", "ordfeminine", "guillemotleft","logicalnot", ".notdef", "registered", "macron",
		"degree", "plusminus", ".notdef", "threesuperior","acute", "mu", "paragraph", "periodcentered",
		"cedilla", "onesuperior", "ordmasculine", "guillemotright","onequarter", "onehalf", "threequarters", "questiondown",
		"Agrave", "Aacute", "Acircumflex", "Atilde","Adieresis", "Aring", "AE", "Ccedilla",
		"Egrave", "Eacute", "Ecircumflex", "Edieresis","Igrave", "Iacute", "Icircumflex", "Idieresis",
		"Eth", "Ntilde", "Ograve", "Oacute","Ocircumflex", "Otilde", "Odieresis", "multiply",
		"Oslash", "Ugrave", "Uacute", "Ucircumflex","Udieresis", "Yacute", "Thorn", "germandbls",
		"agrave", "aacute", "acircumflex", "atilde","adieresis", "aring", "ae", "ccedilla",
		"egrave", "eacute", "ecircumflex", "edieresis","igrave", "iacute", "icircumflex", "idieresis",
		"eth", "ntilde", "ograve", "oacute","ocircumflex", "otilde", "odieresis", "divide",
		"oslash", "ugrave", "uacute", "ucircumflex","udieresis", "yacute", "thorn", "ydieresis"
};

WinAnsiEncoding::WinAnsiEncoding(void)
{
}

WinAnsiEncoding::~WinAnsiEncoding(void)
{
}

BoolAndByte WinAnsiEncoding::Encode(wchar_t inUnicodeCharacter)
{ 
	BoolAndByte result(true,0);

	if(	betweenIncluding<wchar_t>(inUnicodeCharacter,0x00,0x17) ||
		betweenIncluding<wchar_t>(inUnicodeCharacter,0x20,0x7E) ||
		betweenIncluding<wchar_t>(inUnicodeCharacter,0xA1,0xB1) ||
		betweenIncluding<wchar_t>(inUnicodeCharacter,0xB3,0xFF))
	{
		result.second = (char)inUnicodeCharacter;
	}
	else 
	{
		switch(inUnicodeCharacter)
		{
			case 0x02C6:
				result.second = 0x88;
				break;
			case 0x02DC:
				result.second = 0x98;
				break;
			case 0x2022:
				result.second = 0x95;
				break;
			case 0x2020:
				result.second = 0x86;
				break;
			case 0x2021:
				result.second = 0x87;
				break;
			case 0x2026:
				result.second = 0x85;
				break;
			case 0x2014:
				result.second = 0x97;
				break;
			case 0x2013:
				result.second = 0x96;
				break;
			case 0x192:
				result.second = 0x86;
				break;
			case 0x2039:
				result.second = 0x8B;
				break;
			case 0x203A:
				result.second = 0x9B;
				break;
			case 0x2030:
				result.second = 0x89;
				break;
			case 0x201E:
				result.second = 0x84;
				break;
			case 0x201C:
				result.second = 0x93;
				break;
			case 0x201D:
				result.second = 0x94;
				break;
			case 0x2018:
				result.second = 0x91;
				break;
			case 0x2019:
				result.second = 0x92;
				break;
			case 0x201A:
				result.second = 0x82;
				break;
			case 0x2122:
				result.second = 0x99;
				break;
			case 0x152:
				result.second = 0x8C;
				break;
			case 0x160:
				result.second = 0x8A;
				break;
			case 0x178:
				result.second = 0x9F;
				break;
			case 0x17D:
				result.second = 0x8E;
				break;
			case 0x153:
				result.second = 0x9c;
				break;
			case 0x161:
				result.second = 0x9A;
				break;
			case 0x17E:
				result.second = 0x9e;
				break;
			case 0x20AC:
				result.second = 0x80;
				break;
			default:
				result.first = false;
		}
	}
	return result;	
}

const char* WinAnsiEncoding::GetEncodedGlyphName(IOBasicTypes::Byte inEncodedCharacter)
{
	return scGlyphNames[inEncodedCharacter];
}