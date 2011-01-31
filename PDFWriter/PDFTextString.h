/*
   Source File : PDFTextString.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
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
	bool ConvertUTF16ToPDFDocEncoding(const wstring& inStringToConvert,OutputStringBufferStream& refResult);
	void ConvertUTF16ToUTF16BE(const wstring& inStringToConvert,OutputStringBufferStream& refResult);
};
