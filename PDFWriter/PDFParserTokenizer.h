/*
   Source File : PDFParserTokenizer.h


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

   
	PDFParserTokenizer reads tokens from a stream whic is PDF.
	very similar in implementation to InputPFBDecodeStream, without the part
	of acting as a stream. should consider refactoring if makes semantical sense.
*/
#pragma once

#include "IOBasicTypes.h"
#include "EStatusCode.h"

#include <utility>
#include <string>

using namespace std;
using namespace IOBasicTypes;

class IByteReader;

typedef pair<bool,string> BoolAndString;


class PDFParserTokenizer
{
public:
	PDFParserTokenizer(void);
	~PDFParserTokenizer(void);


	// Assign the stream to read from (does not take ownership of the stream)
	void SetReadStream(IByteReader* inSourceStream);

	// Get the next avialable PDF token. return result returns whether
	// token retreive was successful and the token. Token retrieval may be unsuccesful if
	// the end of file was reached and no token was recorded, or if read failure occured.
	// tokens may be:
	// 1. Comments
	// 2. Strings (hex or literal)
	// 3. Dictionary start and end markers
	// 4. Array start and end markers
	// 5. postscript (type 4) function start and end delimeters (curly brackets)
	// 6. Any othr entity separated from other by space or token delimeters (or eof)
	BoolAndString GetNextToken();

private:

	IByteReader* mStream;
	bool mHasTokenBuffer;
	Byte mTokenBuffer;


	void ResetReadState();
	void SkipTillToken();

	// failure in GetNextByteForToken actually marks a true read failure, if you checked end of file before calling it...
	EStatusCode GetNextByteForToken(Byte& outByte);

	bool IsPDFWhiteSpace(Byte inCharacter);
	void SaveTokenBuffer(Byte inToSave);
	bool IsPDFEntityBreaker(Byte inCharacter);

};
