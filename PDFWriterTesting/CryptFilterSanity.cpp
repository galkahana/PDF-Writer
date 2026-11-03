/*
   Source File : CryptFilterSanity.cpp


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


   Regression test for crashes in DecryptionHelper on malformed encryption input.
   Each material is a hand-made one page document, encrypted with empty passwords, whose
   page content is "BT /F1 12 Tf 72 700 Td (Hello) Tj ET":

   - CryptFilterWithoutCFM.pdf: a crypt filter without /CFM (optional, None by default).
     Setup dereferenced the missing name.
   - CryptFilterNotDictionary.pdf: a /CF entry that is a name, not a dictionary. The failed
     cast released an object that the /CF dictionary still owned (use after free).
   - CryptFilterIndirect.pdf: a /CF entry given by reference, to an AESV2 filter. Same
     release as above, and the filter was ignored, so the content was not decrypted.
   - CryptStreamWithoutName.pdf: a stream with /Filter /Crypt and /DecodeParms without
     /Name (optional, Identity by default). GetCryptForStream dereferenced the missing name.
*/
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "InputFile.h"
#include "EStatusCode.h"
#include "Trace.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

static EStatusCode ReadFirstPageContent(const string& inSourcePath, string& outContent) {
	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(inSourcePath) != eSuccess)
		return eFailure;
	if(parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess)
		return eFailure;

	RefCountPtr<PDFDictionary> page(parser.ParsePage(0));
	if(!page)
		return eFailure;

	// reading /Annots parses whatever the xref says is there
	RefCountPtr<PDFObject> annots(parser.QueryDictionaryObject(page.GetPtr(), "Annots"));

	PDFObjectCastPtr<PDFStreamInput> contents(parser.QueryDictionaryObject(page.GetPtr(), "Contents"));
	if(!contents)
		return eFailure;

	IByteReader* reader = parser.StartReadingFromStream(contents.GetPtr());
	if(!reader)
		return eFailure;

	Byte buffer[1024];
	while(reader->NotEnded()) {
		LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
		outContent.append((const char*)buffer, readAmount);
	}
	delete reader;
	return eSuccess;
}

static bool CheckFile(char* argv[], const string& inFileName) {
	string content;
	if(ReadFirstPageContent(BuildRelativeInputPath(argv, inFileName), content) != eSuccess) {
		cout << "CryptFilterSanity: failed to read the page content of " << inFileName << endl;
		return false;
	}
	if(content.find("(Hello) Tj") == string::npos) {
		cout << "CryptFilterSanity: unexpected page content in " << inFileName << endl;
		return false;
	}
	return true;
}

int CryptFilterSanity(int argc, char* argv[]) {
	Trace::DefaultTrace().SetLogSettings(BuildRelativeOutputPath(argv, "CryptFilterSanityLog.txt"), true, true);

	bool ok = true;
	ok = CheckFile(argv, "CryptFilterWithoutCFM.pdf") && ok;
	ok = CheckFile(argv, "CryptFilterNotDictionary.pdf") && ok;
	ok = CheckFile(argv, "CryptFilterIndirect.pdf") && ok;
	ok = CheckFile(argv, "CryptStreamWithoutName.pdf") && ok;

	return ok ? 0 : 1;
}
