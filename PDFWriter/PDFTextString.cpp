#include "PDFTextString.h"
#include "OutputStringBufferStream.h"
#include "PDFDocEncoding.h"

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
	BoolAndByte encodingResult;
	PDFDocEncoding pdfDocEncoding;
	bool PDFEncodingOK = true;

	wstring::const_iterator it = inStringToConvert.begin();
	for(;it != inStringToConvert.end() && PDFEncodingOK;++it)
	{
		encodingResult = pdfDocEncoding.Encode(*it);
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

