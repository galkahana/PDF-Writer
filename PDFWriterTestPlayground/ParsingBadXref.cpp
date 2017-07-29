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
#include "ParsingBadXref.h"
#include "PDFParser.h"
#include "InputFile.h"
#include "EStatusCode.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace PDFHummus;


ParsingBadXref::ParsingBadXref() {

}

ParsingBadXref::~ParsingBadXref() {

}

static EStatusCode openPDF(const string& path);

EStatusCode openPDF(const string& path) {
	PDFParser parser;
	InputFile pdfFile;
	EStatusCode status = pdfFile.OpenFile(path);
	if (status != eSuccess) {
		std::cout << "Invalid path: " << path.c_str() << std::endl;
		return status;
	}

	status = parser.StartPDFParsing(pdfFile.GetInputStream());
	if (status != eSuccess) {
		std::cout << "Failed at start parsing" << std::endl;
	}
	return status;
}

EStatusCode ParsingBadXref::Run(const TestConfiguration& inTestConfiguration) {
	EStatusCode status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/test_bad_xref.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing test_bad_xref.pdf" << std::endl;
		return status;
	}
	return status;
}

ADD_CATEGORIZED_TEST(ParsingBadXref, "Parsing")