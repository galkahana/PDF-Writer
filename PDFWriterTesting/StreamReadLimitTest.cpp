/*
   Source File : StreamReadLimitTest.cpp


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


   Test for PDFParsingOptions::MaxStreamReadSize and PDFParsingOptions::ReadInterrupt.
   StreamReadLimit.pdf is about 1 KB. Its content stream (object 4) is Flate encoded twice,
   and decodes to "BT /F1 12 Tf 72 700 Td (Hello) Tj ET", 16 MiB of spaces, then a second text
   object. Page 0 has it as /Contents, page 1 lists it eight times in a /Contents array.
*/
#include "PDFParser.h"
#include "PDFParsingOptions.h"
#include "IReadInterrupt.h"
#include "PDFArray.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFStreamInput.h"
#include "PDFObjectParser.h"
#include "PDFLiteralString.h"
#include "IByteReader.h"
#include "InputFile.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

static const LongBufferSizeType scDecodedSize = 37 + (16 << 20) + 37;
static const LongBufferSizeType scLimit = 1 << 20;

class StopAtFirstAsk : public IReadInterrupt
{
public:
	StopAtFirstAsk() { mAsked = 0; }
	virtual bool ShouldStop() { ++mAsked; return true; }

	unsigned long mAsked;
};

static LongBufferSizeType ReadPageContent(PDFParser& inParser, string& outHead) {
	RefCountPtr<PDFDictionary> page(inParser.ParsePage(0));
	if(!page)
		return 0;
	PDFObjectCastPtr<PDFStreamInput> contents(inParser.QueryDictionaryObject(page.GetPtr(), "Contents"));
	if(!contents)
		return 0;
	IByteReader* reader = inParser.StartReadingFromStream(contents.GetPtr());
	if(!reader)
		return 0;

	LongBufferSizeType total = 0;
	Byte buffer[4096];
	while(reader->NotEnded()) {
		LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
		if(total == 0)
			outHead.assign((const char*)buffer, readAmount);
		total += readAmount;
	}
	delete reader;
	return total;
}

static unsigned long CountHelloStringsInPage1(PDFParser& inParser) {
	RefCountPtr<PDFDictionary> page(inParser.ParsePage(1));
	if(!page)
		return 0;
	PDFObjectCastPtr<PDFArray> contents(inParser.QueryDictionaryObject(page.GetPtr(), "Contents"));
	if(!contents)
		return 0;
	PDFObjectParser* objectsParser = inParser.StartReadingObjectsFromStreams(contents.GetPtr());
	if(!objectsParser)
		return 0;

	unsigned long count = 0;
	PDFObject* anObject = objectsParser->ParseNewObject();
	while(anObject) {
		if(anObject->GetType() == PDFObject::ePDFObjectLiteralString && ((PDFLiteralString*)anObject)->GetValue() == "Hello")
			++count;
		anObject->Release();
		anObject = objectsParser->ParseNewObject();
	}
	delete objectsParser;
	return count;
}

static bool StartParsing(PDFParser& inParser, InputFile& inFile, char* argv[], const PDFParsingOptions& inOptions) {
	if(inFile.OpenFile(BuildRelativeInputPath(argv, "StreamReadLimit.pdf")) != eSuccess)
		return false;
	return inParser.StartPDFParsing(inFile.GetInputStream(), inOptions) == eSuccess;
}

int StreamReadLimitTest(int argc, char* argv[]) {
	// by default nothing is limited
	{
		PDFParser parser;
		InputFile pdfFile;
		if(!StartParsing(parser, pdfFile, argv, PDFParsingOptions::DefaultPDFParsingOptions())) {
			cout << "StreamReadLimitTest: failed to start parsing StreamReadLimit.pdf" << endl;
			return 1;
		}
		string head;
		LongBufferSizeType total = ReadPageContent(parser, head);
		if(total != scDecodedSize || parser.GetTruncatedStreamsCount() != 0) {
			cout << "StreamReadLimitTest: with no limit, expected " << scDecodedSize << " bytes, read " << total << endl;
			return 1;
		}
	}

	// a limit ends the stream there, and counts it
	{
		PDFParser parser;
		InputFile pdfFile;
		PDFParsingOptions options;
		options.MaxStreamReadSize = scLimit;
		if(!StartParsing(parser, pdfFile, argv, options)) {
			cout << "StreamReadLimitTest: failed to start parsing StreamReadLimit.pdf with a limit" << endl;
			return 1;
		}
		string head;
		LongBufferSizeType total = ReadPageContent(parser, head);
		if(total != scLimit || head.find("(Hello) Tj") == string::npos || parser.GetTruncatedStreamsCount() != 1) {
			cout << "StreamReadLimitTest: with a limit, expected " << scLimit << " bytes and one truncated stream, read " << total <<
				" with " << parser.GetTruncatedStreamsCount() << " truncated" << endl;
			return 1;
		}

		// a /Contents array is limited as a whole, so only the first copy of the stream is read
		unsigned long hellos = CountHelloStringsInPage1(parser);
		if(hellos != 1) {
			cout << "StreamReadLimitTest: with a limit, expected the /Contents array to read one (Hello), read " << hellos << endl;
			return 1;
		}
	}

	// the interrupt stops a read when asked
	{
		PDFParser parser;
		InputFile pdfFile;
		StopAtFirstAsk interrupt;
		PDFParsingOptions options;
		options.ReadInterrupt = &interrupt;
		if(!StartParsing(parser, pdfFile, argv, options)) {
			cout << "StreamReadLimitTest: failed to start parsing StreamReadLimit.pdf with an interrupt" << endl;
			return 1;
		}
		string head;
		LongBufferSizeType total = ReadPageContent(parser, head);
		if(interrupt.mAsked != 1 || total >= scDecodedSize || parser.GetInterruptedStreamsCount() != 1 || parser.GetTruncatedStreamsCount() != 0) {
			cout << "StreamReadLimitTest: with an interrupt, expected one stopped stream, read " << total << " bytes, asked " <<
				interrupt.mAsked << " times, " << parser.GetInterruptedStreamsCount() << " interrupted" << endl;
			return 1;
		}
	}

	return 0;
}
