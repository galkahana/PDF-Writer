#include "PDFTextString.h"
#include "OutputStringBufferStream.h"
#include "BetweenIncluding.h"

using namespace IOBasicTypes;

PDFTextString::PDFTextString(void)
{
}

PDFTextString::PDFTextString(const wstring inString)
{
	ConvertFromUTF16(inString);
}

PDFTextString::~PDFTextString(void)
{
}

const PDFTextString PDFTextString::Empty;

PDFTextString& PDFTextString::FromUTF16(const wstring& inString)
{
	ConvertFromUTF16(inString);
	return *this;
}

bool PDFTextString::IsEmpty() const
{
	return mTextString.size() == 0;
}

const string& PDFTextString::ToString() const
{
	return mTextString;
}

bool PDFTextString::operator==(const PDFTextString& inString) const
{
	return mTextString == inString.mTextString;
}

PDFTextString& PDFTextString::operator=(const PDFTextString& inString)
{
	mTextString = inString.mTextString;
	return *this;
}

static const Byte scBigEndianMark[]= {0xFE,0xFF};

void PDFTextString::ConvertFromUTF16(const wstring& inStringToConvert)
{
	OutputStringBufferStream aStringStream;

	if(!ConvertUTF16ToPDFDocEncoding(inStringToConvert,aStringStream))
	{
		aStringStream.Reset();
		ConvertUTF16ToUTF16BE(inStringToConvert,aStringStream);
	}
	mTextString = aStringStream.ToString();
}

bool PDFTextString::ConvertUTF16ToPDFDocEncoding(const wstring& inStringToConvert,OutputStringBufferStream& refResult)
{
	ConvertToPDFDocEncodingResult encodingResult;
	bool PDFEncodingOK = true;

	wstring::const_iterator it = inStringToConvert.begin();
	for(;it != inStringToConvert.end() && PDFEncodingOK;++it)
	{
		encodingResult = ConvertToPDFDocEncoding(*it);
		if(encodingResult.first)
			refResult.Write(&(encodingResult.second),1);
		else
			PDFEncodingOK = false;
	}

	return PDFEncodingOK;
}

void PDFTextString::ConvertUTF16ToUTF16BE(const wstring& inStringToConvert,OutputStringBufferStream& refResult)
{
	Byte bigEndian[2];

	refResult.Write(scBigEndianMark,2);
	wstring::const_iterator it = inStringToConvert.begin();
	for(;it != inStringToConvert.end();++it)
	{
		bigEndian[0] = Byte((*it)>>8);
		bigEndian[1] = Byte((*it) & 0xFF);
		refResult.Write(bigEndian,2);
	}
}

ConvertToPDFDocEncodingResult PDFTextString::ConvertToPDFDocEncoding(wchar_t inCharacterToConvert)
{
	ConvertToPDFDocEncodingResult result(true,0);

	if(	betweenIncluding<wchar_t>(inCharacterToConvert,0x00,0x17) ||
		betweenIncluding<wchar_t>(inCharacterToConvert,0x20,0x7E) ||
		betweenIncluding<wchar_t>(inCharacterToConvert,0xA1,0xFF))
	{
		result.second = (char)inCharacterToConvert;
	}
	else 
	{
		switch(inCharacterToConvert)
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