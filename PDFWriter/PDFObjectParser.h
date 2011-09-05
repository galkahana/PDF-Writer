/*
   Source File : PDFObjectParser.h


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

#include "PDFParserTokenizer.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"
#include "IReadPositionProvider.h"

#include <list>
#include <string>
#include <utility>

class PDFObject;
class IByteReader;

using namespace std;

typedef pair<bool,IOBasicTypes::Byte> BoolAndByte;
typedef list<string> StringList;

class PDFObjectParser
{
public:
	PDFObjectParser(void);
	~PDFObjectParser(void);

	
	// Assign the stream to read from (does not take ownership of the stream)
	void SetReadStream(IByteReader* inSourceStream,IReadPositionProvider* inCurrentPositionProvider);

	PDFObject* ParseNewObject();

	// calls this when changing underlying stream position
	void ResetReadState();

private:
	PDFParserTokenizer mTokenizer;
	StringList mTokenBuffer;
	IByteReader* mStream;
	IReadPositionProvider* mCurrentPositionProvider;

	bool GetNextToken(string& outToken);
	void SaveTokenToBuffer(string& inToken);
	void ReturnTokenToBuffer(string& inToken);

	bool IsBoolean(const string& inToken);
	PDFObject* ParseBoolean(const string& inToken);

	bool IsLiteralString(const string& inToken);
	PDFObject* ParseLiteralString(const string& inToken);

	bool IsHexadecimalString(const string& inToken);
	PDFObject* ParseHexadecimalString(const string& inToken);

	bool IsNull(const string& inToken);

	bool IsName(const string& inToken);
	PDFObject* ParseName(const string& inToken);

	bool IsNumber(const string& inToken);
	PDFObject* ParseNumber(const string& inToken);

	bool IsArray(const string& inToken);
	PDFObject* ParseArray();

	bool IsDictionary(const string& inToken);
	PDFObject* ParseDictionary();

	bool IsComment(const string& inToken);

	BoolAndByte GetHexValue(IOBasicTypes::Byte inValue);


};
