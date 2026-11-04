/*
   Source File : PredictorWithoutSamplesTest.cpp


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


   Regression test for predictor parameters that describe no row.
   PredictorWithoutSamples.pdf holds streams (objects 4 to 11; LZW for the TIFF predictor, Flate
   for the PNG one) with these /DecodeParms:
   - 4 to 6: /Predictor 2 with /Columns 0, /Colors 0, /Columns -1
   - 7 to 9: /Predictor 12 with /Columns 0, /Colors 0, /Colors -1
   - 10, 11: /Predictor 2 and 12 with /Colors 8 /Columns 2^61, a row whose size overflows
   The predictors refuse such parameters in Assign and keep no row buffer, but the parser took
   the stream as built. The TIFF predictor then read a row of uninitialized size into the null
   buffer, and the PNG predictor kept no source, so the decode chain under it leaked.
*/
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "InputFile.h"
#include "InputPredictorPNGOptimumStream.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

class DeletionTrackingReader : public IByteReader
{
public:
	DeletionTrackingReader(bool* outDeleted) { mDeleted = outDeleted; }
	virtual ~DeletionTrackingReader() { *mDeleted = true; }
	virtual LongBufferSizeType Read(Byte* inBuffer, LongBufferSizeType inBufferSize) { return 0; }
	virtual bool NotEnded() { return false; }

private:
	bool* mDeleted;
};

// opens the stream, and reads it to the end. returns false if it did not open
static bool ReadStream(PDFParser& inParser, ObjectIDType inObjectID, LongBufferSizeType& outReadSize) {
	outReadSize = 0;
	PDFObjectCastPtr<PDFStreamInput> stream(inParser.ParseNewObject(inObjectID));
	if(!stream)
		return false;
	IByteReader* reader = inParser.StartReadingFromStream(stream.GetPtr());
	if(!reader)
		return false;
	Byte buffer[256];
	while(reader->NotEnded()) {
		LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
		if(readAmount == 0)
			break;
		outReadSize += readAmount;
	}
	delete reader;
	return true;
}

int PredictorWithoutSamplesTest(int argc, char* argv[]) {
	bool ok = true;

	// the PNG predictor owns its source even when it refuses the parameters
	{
		bool deleted = false;
		InputPredictorPNGOptimumStream* predictor = new InputPredictorPNGOptimumStream(new DeletionTrackingReader(&deleted),
																					  8, 8, (LongBufferSizeType)1 << 61);
		delete predictor;
		if(!deleted) {
			cout << "PredictorWithoutSamplesTest: the PNG predictor did not free its source after refusing its parameters" << endl;
			ok = false;
		}
	}

	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(BuildRelativeInputPath(argv, "PredictorWithoutSamples.pdf")) != eSuccess ||
		parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess) {
		cout << "PredictorWithoutSamplesTest: failed to start parsing PredictorWithoutSamples.pdf" << endl;
		return 1;
	}

	// a count below 1: the stream does not open
	ObjectIDType noSamples[] = {7, 8, 9, 4, 5, 6};
	for(size_t i = 0; i < sizeof(noSamples) / sizeof(noSamples[0]); ++i) {
		LongBufferSizeType readSize;
		if(ReadStream(parser, noSamples[i], readSize)) {
			cout << "PredictorWithoutSamplesTest: object " << noSamples[i] << " opened, expected a failure" << endl;
			ok = false;
		}
	}

	// a row too large to describe: the predictor gives nothing
	for(ObjectIDType objectID = 10; objectID <= 11; ++objectID) {
		LongBufferSizeType readSize;
		ReadStream(parser, objectID, readSize);
		if(readSize != 0) {
			cout << "PredictorWithoutSamplesTest: object " << objectID << " gave " << readSize << " bytes, expected none" << endl;
			ok = false;
		}
	}

	return ok ? 0 : 1;
}
