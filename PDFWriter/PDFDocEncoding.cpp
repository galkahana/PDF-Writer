#include "PDFDocEncoding.h"
#include "BetweenIncluding.h"

PDFDocEncoding::PDFDocEncoding(void)
{
}

PDFDocEncoding::~PDFDocEncoding(void)
{
}

BoolAndByte PDFDocEncoding::Encode(wchar_t inUnicodeCharacter)
{
	BoolAndByte result(true,0);

	if(	betweenIncluding<wchar_t>(inUnicodeCharacter,0x00,0x17) ||
		betweenIncluding<wchar_t>(inUnicodeCharacter,0x20,0x7E) ||
		betweenIncluding<wchar_t>(inUnicodeCharacter,0xA1,0xFF))
	{
		result.second = (char)inUnicodeCharacter;
	}
	else 
	{
		switch(inUnicodeCharacter)
		{
			case 0x02D8:
				result.second = 0x18;
				break;
			case 0x02C7:
				result.second = 0x19;
				break;
			case 0x02C6:
				result.second = 0x1a;
				break;
			case 0x02D9:
				result.second = 0x1b;
				break;
			case 0x02DD:
				result.second = 0x1c;
				break;
			case 0x02DB:
				result.second = 0x1d;
				break;
			case 0x02DA:
				result.second = 0x1e;
				break;
			case 0x02DC:
				result.second = 0x1f;
				break;
			case 0x2022:
				result.second = 0x80;
				break;
			case 0x2020:
				result.second = 0x81;
				break;
			case 0x2021:
				result.second = 0x82;
				break;
			case 0x2026:
				result.second = 0x83;
				break;
			case 0x2014:
				result.second = 0x84;
				break;
			case 0x2013:
				result.second = 0x85;
				break;
			case 0x192:
				result.second = 0x86;
				break;
			case 0x2044:
				result.second = 0x87;
				break;
			case 0x2039:
				result.second = 0x88;
				break;
			case 0x203A:
				result.second = 0x89;
				break;
			case 0x2212:
				result.second = 0x8a;
				break;
			case 0x2030:
				result.second = 0x8b;
				break;
			case 0x201E:
				result.second = 0x8c;
				break;
			case 0x201C:
				result.second = 0x8d;
				break;
			case 0x201D:
				result.second = 0x8e;
				break;
			case 0x2018:
				result.second = 0x8f;
				break;
			case 0x2019:
				result.second = 0x90;
				break;
			case 0x201A:
				result.second = 0x91;
				break;
			case 0x2122:
				result.second = 0x92;
				break;
			case 0xFB01:
				result.second = 0x93;
				break;
			case 0xFB02:
				result.second = 0x94;
				break;
			case 0x141:
				result.second = 0x95;
				break;
			case 0x152:
				result.second = 0x96;
				break;
			case 0x160:
				result.second = 0x97;
				break;
			case 0x178:
				result.second = 0x98;
				break;
			case 0x17D:
				result.second = 0x99;
				break;
			case 0x131:
				result.second = 0x9a;
				break;
			case 0x142:
				result.second = 0x9b;
				break;
			case 0x153:
				result.second = 0x9c;
				break;
			case 0x161:
				result.second = 0x9d;
				break;
			case 0x17E:
				result.second = 0x9e;
				break;
			case 0x20AC:
				result.second = 0xa0;
				break;
			default:
				result.first = false;
		}
	}
	return result;
}
