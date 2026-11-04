/*
   Source File : TIFFPredictorTest.cpp


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


   Test for decoding with the TIFF predictor (/Predictor 2).
   TIFFPredictor.pdf holds three Flate streams with TIFF predicted samples:
   - object 4: 8 bits, 1 color, 4 columns, 3 rows
   - object 5: 4 bits, 1 color, 3 columns, 3 rows. A row is 12 bits, so 2 bytes, the last one padded
   - object 6: 16 bits, 3 colors, 1 column, 2 rows
   Each stream is read to the end with a buffer larger than the image, and compared with the
   samples it was made from.
*/
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "InputFile.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

static const Byte scExpected8Bits[] = {0x0a, 0x14, 0x1e, 0x28, 0x05, 0x05, 0x06, 0xfa, 0x00, 0xff, 0x00, 0xff};

static bool CheckStream(PDFParser& inParser, ObjectIDType inObjectID, const Byte* inExpected, size_t inExpectedSize) {
	PDFObjectCastPtr<PDFStreamInput> stream(inParser.ParseNewObject(inObjectID));
	if(!stream) {
		cout << "TIFFPredictorTest: failed to parse object " << inObjectID << endl;
		return false;
	}
	IByteReader* reader = inParser.StartReadingFromStream(stream.GetPtr());
	if(!reader) {
		cout << "TIFFPredictorTest: failed to open object " << inObjectID << endl;
		return false;
	}
	string decoded;
	Byte buffer[4096];
	while(reader->NotEnded()) {
		LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
		if(readAmount == 0)
			break;
		decoded.append((const char*)buffer, readAmount);
	}
	delete reader;

	if(decoded != string((const char*)inExpected, inExpectedSize)) {
		cout << "TIFFPredictorTest: object " << inObjectID << " did not decode to its samples (" << decoded.size() <<
			" bytes, " << inExpectedSize << " expected)" << endl;
		return false;
	}
	return true;
}

int TIFFPredictorTest(int argc, char* argv[]) {
	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(BuildRelativeInputPath(argv, "TIFFPredictor.pdf")) != eSuccess ||
		parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess) {
		cout << "TIFFPredictorTest: failed to start parsing TIFFPredictor.pdf" << endl;
		return 1;
	}

	bool ok = true;
	ok = CheckStream(parser, 4, scExpected8Bits, sizeof(scExpected8Bits)) && ok;

	return ok ? 0 : 1;
}
