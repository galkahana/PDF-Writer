#pragma once
/*
	Representing a PDF text string. In accordance with PDF specifications 1.7, section 3.8.1 of text string. 
	encodes a double byte Unicode text to a PDF text string, 
	either using PDFDocEncoded or UTF-16BE encoded string with a leading byte order marker. 
	if possible, the string is encoded using PDFDocEncoded. otherwise UTF16BE is used.
	when encoding, it is made sure that all charachters that require escaping, get escaped.
*/

#include "OutputStringBufferStream.h"

#include <string>
#include <utility>

using namespace std;

typedef pair<bool,IOBasicTypes::Byte> ConvertToPDFDocEncodingResult;

class PDFTextString
{
public:
	
	PDFTextString();
	PDFTextString(const wstring inString);
	~PDFTextString(void);


	PDFTextString& FromUTF16(const wstring& inString); // will try first to convert to PDFDocEncoded, and if unable, to UTF16BE

	bool IsEmpty() const;
	const string& ToString() const; // string representation of the PDFTextString, to be used for writing

	bool operator==(const PDFTextString& inString) const; //equality check is string equality based
	PDFTextString& operator=(const PDFTextString& inString);

	static const PDFTextString Empty;

private:

	string mTextString;

	void ConvertFromUTF16(const wstring& inStringToConvert);
	ConvertToPDFDocEncodingResult ConvertToPDFDocEncoding(wchar_t inCharacterToConvert);
	bool ConvertUTF16ToPDFDocEncoding(const wstring& inStringToConvert,OutputStringBufferStream& refResult);
	void ConvertUTF16ToUTF16BE(const wstring& inStringToConvert,OutputStringBufferStream& refResult);
};
