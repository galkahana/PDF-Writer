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
#include "FlateObjectDecodeTest.h"
#include "PDFParser.h"
#include "InputFile.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "EStatusCode.h"

#include <iostream>
#include <fstream>
#include <string>


using namespace std;
using namespace PDFHummus;

EStatusCode decodeStream(const string sourcePath, const string targetPath);

#define BUF_SIZE 1024

EStatusCode decodeStream(const string sourcePath, const string targetPath) {
	PDFParser parser;
	InputFile pdfFile;
	EStatusCode status = pdfFile.OpenFile(sourcePath);
	if (status == eSuccess) {
		status = parser.StartPDFParsing(pdfFile.GetInputStream());
		if (status == eSuccess) {
			// Parse image object
			PDFObject* streamObj = parser.ParseNewObject(7);
			if (streamObj != NULL
				&& streamObj->GetType() == PDFObject::ePDFObjectStream) {
				PDFStreamInput* stream = ((PDFStreamInput*)streamObj);
				IByteReader* reader = parser.StartReadingFromStream(stream);
				if (!reader) {
					cout << "Couldn't create reader\n";
				}

				ofstream os(targetPath.c_str(), ofstream::binary);
				Byte buffer[1024];
				LongBufferSizeType total = 0;
				while (reader->NotEnded()) {
					LongBufferSizeType readAmount = reader->Read(buffer, BUF_SIZE);
					os.write((char*)buffer, readAmount);
					total += readAmount;
				}

				os.close();

				cout << "Total read: " << total << "\n";
				cout << "Expected read: " << 120000 << "\n";

				return total == 120000 ? eSuccess : eFailure;
			}
			else
				return eFailure;
		}
		else
			return status;
	}
	else
		return status;
}

FlateObjectDecodeTest::FlateObjectDecodeTest() {

}

FlateObjectDecodeTest::~FlateObjectDecodeTest() {

}

EStatusCode FlateObjectDecodeTest::Run(const TestConfiguration& inTestConfiguration) {
	return decodeStream(
		RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/test2.pdf"),
		RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "image.data")
	);
}

ADD_CATEGORIZED_TEST(FlateObjectDecodeTest, "Parsing")