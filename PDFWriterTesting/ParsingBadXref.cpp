/*
Source File : FlateObjectDecodeTest.cpp


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
#include "PDFParser.h"
#include "InputFile.h"
#include "EStatusCode.h"

#include <iostream>
#include <fstream>
#include <string>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

int ParsingBadXref(int arg, char* argv[]) {
	PDFParser parser;
	InputFile pdfFile;

	string path = BuildRelativeInputPath(argv, "test_bad_xref.pdf");
	EStatusCode status = pdfFile.OpenFile(path);
	if (status != eSuccess) {
		std::cout << "Invalid path: " << path.c_str() << std::endl;
		return 1;
	}

	// The xref pointer is to position 173, which is 1 char AFTER the "xref" symbol start
	// so "ref" is parsed. for safety the parser marks it as non parsable and exists...so failing to parse is expected
	status = parser.StartPDFParsing(pdfFile.GetInputStream());
	if (status != eSuccess) {
		std::cout << "Correctly failed at start parsing" << std::endl;
		return 0;

	}


	return 1; // should have failed
}
