/*
   Source File : PredictorRowLimitTest.cpp


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


   Test for predictor rows past PDFParsingOptions::MaxStreamReadSize, and for predictor buffers
   that cannot be allocated. PredictorRowLimit.pdf holds Flate streams of 16 bytes (objects 4 to 8)
   with these /DecodeParms:
   - 4, 5: /Predictor 12 and 2 with /Colors 4 /Columns 10000000, a 40 MB row
   - 6, 7: /Predictor 12 and 2 with /Columns 2^60, a row no allocation can give
   - 8: /Predictor 12 with /Columns 4, two valid rows
   The predictors allocate their row buffers before reading any data, so a few bytes of stream
   made the parser allocate (and clear) what /DecodeParms asked for, or throw std::bad_alloc out
   of StartReadingFromStream.
*/
#include "PDFParser.h"
#include "PDFParsingOptions.h"
#include "PDFObjectCast.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "InputFile.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

// AddressSanitizer reports a failed allocation as an error instead of throwing std::bad_alloc
#if defined(__SANITIZE_ADDRESS__)
#define PREDICTOR_ROW_LIMIT_TEST_ASAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define PREDICTOR_ROW_LIMIT_TEST_ASAN 1
#endif
#endif

static bool OpensStream(PDFParser& inParser, ObjectIDType inObjectID) {
	PDFObjectCastPtr<PDFStreamInput> stream(inParser.ParseNewObject(inObjectID));
	if(!stream)
		return false;
	IByteReader* reader = inParser.StartReadingFromStream(stream.GetPtr());
	if(!reader)
		return false;
	delete reader;
	return true;
}

static bool StartParsing(PDFParser& inParser, InputFile& inFile, char* argv[], const PDFParsingOptions& inOptions) {
	if(inFile.OpenFile(BuildRelativeInputPath(argv, "PredictorRowLimit.pdf")) != eSuccess)
		return false;
	return inParser.StartPDFParsing(inFile.GetInputStream(), inOptions) == eSuccess;
}

int PredictorRowLimitTest(int argc, char* argv[]) {
	bool ok = true;

	// with a read limit, a row past it does not open, and counts as truncated
	{
		PDFParsingOptions options;
		options.MaxStreamReadSize = 1 << 20;
		PDFParser parser;
		InputFile pdfFile;
		if(!StartParsing(parser, pdfFile, argv, options)) {
			cout << "PredictorRowLimitTest: failed to start parsing PredictorRowLimit.pdf" << endl;
			return 1;
		}
		for(ObjectIDType objectID = 4; objectID <= 5; ++objectID) {
			if(OpensStream(parser, objectID)) {
				cout << "PredictorRowLimitTest: object " << objectID << " opened, its row is past the read limit" << endl;
				ok = false;
			}
		}
		if(!OpensStream(parser, 8)) {
			cout << "PredictorRowLimitTest: object 8 did not open under the read limit" << endl;
			ok = false;
		}
		if(parser.GetTruncatedStreamsCount() != 2) {
			cout << "PredictorRowLimitTest: " << parser.GetTruncatedStreamsCount() << " truncated streams, expected 2" << endl;
			ok = false;
		}
	}

#ifndef PREDICTOR_ROW_LIMIT_TEST_ASAN
	// without one, buffers that cannot be allocated make the stream fail to open
	{
		PDFParser parser;
		InputFile pdfFile;
		if(!StartParsing(parser, pdfFile, argv, PDFParsingOptions::DefaultPDFParsingOptions())) {
			cout << "PredictorRowLimitTest: failed to start parsing PredictorRowLimit.pdf" << endl;
			return 1;
		}
		for(ObjectIDType objectID = 6; objectID <= 7; ++objectID) {
			if(OpensStream(parser, objectID)) {
				cout << "PredictorRowLimitTest: object " << objectID << " opened, its row cannot be allocated" << endl;
				ok = false;
			}
		}
	}
#endif

	return ok ? 0 : 1;
}
