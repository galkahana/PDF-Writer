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
#include "ParsingFaulty.h"
#include "PDFParser.h"
#include "PDFPageInput.h"
#include "InputFile.h"
#include "EStatusCode.h"
#include "Trace.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace PDFHummus;


ParsingFaulty::ParsingFaulty() {

}

ParsingFaulty::~ParsingFaulty() {

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

EStatusCode openPDFForNullPageTest(const string& path) {
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

EStatusCode openPDFForRotationTest(const string& path) {
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

EStatusCode ParsingFaulty::Run(const TestConfiguration& inTestConfiguration) {
	Trace::DefaultTrace().SetLogSettings(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "parsingFaultyLog.txt"), true, true);


	EStatusCode status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/test3.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing test3.pdf" << std::endl;
		return status;
	}

	status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/test4.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing test4.pdf" << std::endl;
		return status;
	}

	status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/kids-as-reference.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing kids-as-reference.pdf" << std::endl;
		return status;
	}


	status = openPDFForNullPageTest(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/unexpected.kids.array.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing unexpected.kids.array.pdf" << std::endl;
		return status;
	}

	status = openPDFForRotationTest(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/wrong.rotation.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing wrong.rotation.pdf" << std::endl;
		return status;
	}
	
	status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/1.unfamiliar.entry.type.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing 1.unfamiliar.entry.type.pdf" << std::endl;
		return status;
	}

	status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/2.unfamiliar.entry.type.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing 2.unfamiliar.entry.type.pdf" << std::endl;
		return status;
	}

	status = openPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/nonZeroXref.pdf"));
	if (status != eSuccess) {
		std::cout << "Failed at start parsing nonZeroXref.pdf" << std::endl;
		return status;
	}



	return status;
}

ADD_CATEGORIZED_TEST(ParsingFaulty, "Parsing")