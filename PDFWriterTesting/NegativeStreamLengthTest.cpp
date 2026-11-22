/*
   Source File : NegativeStreamLengthTest.cpp


   Copyright 2026 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


   Regression test for a stream with a negative /Length.
   InputLimitedStream passed the negative limit to its source as an unsigned size, so the
   source copied everything it had into the caller's buffer. InputLZWDecodeStream reads one
   byte at a time into a 1 byte buffer, so NegativeStreamLength.pdf (a page content stream
   with /Filter /LZWDecode /Length -1, followed by 4 KB of file) overflowed the stack.
*/
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "InputFile.h"
#include "InputByteArrayStream.h"
#include "InputLimitedStream.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

int NegativeStreamLengthTest(int argc, char* argv[]) {
	// the stream itself: a negative limit reads nothing
	{
		Byte source[8] = {1, 2, 3, 4, 5, 6, 7, 8};
		InputByteArrayStream sourceStream(source, sizeof(source));
		InputLimitedStream limitedStream(&sourceStream, -1, false);
		Byte buffer[16];
		LongBufferSizeType readAmount = limitedStream.Read(buffer, sizeof(buffer));
		if(readAmount != 0 || limitedStream.NotEnded()) {
			cout << "NegativeStreamLengthTest: a limit of -1 read " << readAmount << " bytes, expected none" << endl;
			return 1;
		}
	}

	// and through the parser, with the LZW decoder on top of it
	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(BuildRelativeInputPath(argv, "NegativeStreamLength.pdf")) != eSuccess ||
		parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess) {
		cout << "NegativeStreamLengthTest: failed to start parsing NegativeStreamLength.pdf" << endl;
		return 1;
	}
	RefCountPtr<PDFDictionary> page(parser.ParsePage(0));
	if(!page) {
		cout << "NegativeStreamLengthTest: failed to parse page 0" << endl;
		return 1;
	}
	PDFObjectCastPtr<PDFStreamInput> contents(parser.QueryDictionaryObject(page.GetPtr(), "Contents"));
	if(!contents) {
		cout << "NegativeStreamLengthTest: failed to parse the page content stream" << endl;
		return 1;
	}
	IByteReader* reader = parser.StartReadingFromStream(contents.GetPtr());
	LongBufferSizeType total = 0;
	if(reader) {
		Byte buffer[1024];
		while(reader->NotEnded()) {
			LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
			if(readAmount == 0)
				break;
			total += readAmount;
		}
		delete reader;
	}
	if(total != 0) {
		cout << "NegativeStreamLengthTest: the content stream with /Length -1 gave " << total << " bytes, expected none" << endl;
		return 1;
	}

	return 0;
}
