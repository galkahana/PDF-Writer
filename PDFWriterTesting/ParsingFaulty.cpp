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
#include "PDFPageInput.h"
#include "InputFile.h"
#include "EStatusCode.h"
#include "Trace.h"

#include <iostream>
#include <fstream>
#include <string>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;


static EStatusCode openPDF(const string& path) {
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

static EStatusCode openPDFForNullPageTest(const string& path) {
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

	RefCountPtr<PDFDictionary> page = parser.ParsePage(33);
	if(page != NULL) {
		status = eFailure;
		std::cout << "Page should be null at 33"<< std::endl;
	}
	return status;
}

static EStatusCode openPDFForRotationTest(const string& path) {
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


	PDFPageInput pageInput(&parser, parser.ParsePage(0));
	if (pageInput.GetRotate() != -90) {
		status = eFailure;
		std::cout << "Page rotation should be -90, but is " << pageInput.GetRotate()<< std::endl;
	}

	return status;
}

int ParsingFaulty(int argc, char* argv[]) {
	Trace::DefaultTrace().SetLogSettings(BuildRelativeOutputPath(argv,  "parsingFaultyLog.txt"), true, true);


	EStatusCode status = openPDF(BuildRelativeInputPath(argv, "test3.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing test3.pdf" << std::endl;
		return status;
	}

	status = openPDF(BuildRelativeInputPath(argv, "test4.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing test4.pdf" << std::endl;
		return status;
	}

	status = openPDF(BuildRelativeInputPath(argv, "kids-as-reference.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing kids-as-reference.pdf" << std::endl;
		return status;
	}


	status = openPDFForNullPageTest(BuildRelativeInputPath(argv, "unexpected.kids.array.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing unexpected.kids.array.pdf" << std::endl;
		return status;
	}

	status = openPDFForRotationTest(BuildRelativeInputPath(argv, "wrong.rotation.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing wrong.rotation.pdf" << std::endl;
		return status;
	}
	
	status = openPDF(BuildRelativeInputPath(argv, "1.unfamiliar.entry.type.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing 1.unfamiliar.entry.type.pdf" << std::endl;
		return status;
	}

	status = openPDF(BuildRelativeInputPath(argv, "2.unfamiliar.entry.type.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing 2.unfamiliar.entry.type.pdf" << std::endl;
		return status;
	}

	status = openPDF(BuildRelativeInputPath(argv, "nonZeroXref.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing nonZeroXref.pdf" << std::endl;
		return status;
	}



	return status == eSuccess ? 0:1;
}
