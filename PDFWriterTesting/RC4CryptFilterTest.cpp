/*
   Source File : RC4CryptFilterTest.cpp


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


   Regression test for reading a V 4 document whose crypt filter is RC4. ISO 32000-1 Table 25
   names that method /CFM /V2. CryptFilterV2.pdf is a hand-made one page document, encrypted
   with empty passwords, whose /StdCF filter is << /CFM /V2 /Length 16 >> and whose page
   content, RC4 encrypted with it, is "BT /F1 12 Tf 72 700 Td (Hello) Tj ET". V2 used to be
   taken for a filter that does not encrypt, and the content was read as ciphertext.
*/
#include "PDFParser.h"
#include "PDFDictionary.h"
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

int RC4CryptFilterTest(int argc, char* argv[]) {
	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(BuildRelativeInputPath(argv, "CryptFilterV2.pdf")) != eSuccess) {
		cout << "RC4CryptFilterTest: failed to open CryptFilterV2.pdf" << endl;
		return 1;
	}
	if(parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess) {
		cout << "RC4CryptFilterTest: failed to start parsing CryptFilterV2.pdf" << endl;
		return 1;
	}

	RefCountPtr<PDFDictionary> page(parser.ParsePage(0));
	if(!page) {
		cout << "RC4CryptFilterTest: failed to parse page 0" << endl;
		return 1;
	}

	PDFObjectCastPtr<PDFStreamInput> contents(parser.QueryDictionaryObject(page.GetPtr(), "Contents"));
	if(!contents) {
		cout << "RC4CryptFilterTest: no page content stream" << endl;
		return 1;
	}

	IByteReader* reader = parser.StartReadingFromStream(contents.GetPtr());
	if(!reader) {
		cout << "RC4CryptFilterTest: failed to read the page content stream" << endl;
		return 1;
	}

	string content;
	Byte buffer[1024];
	while(reader->NotEnded()) {
		LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
		content.append((const char*)buffer, readAmount);
	}
	delete reader;

	if(content.find("(Hello) Tj") == string::npos) {
		cout << "RC4CryptFilterTest: the page content was not decrypted" << endl;
		return 1;
	}

	return 0;
}
