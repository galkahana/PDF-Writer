/*
   Source File : PDFParserTest.cpp


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
#include "PDFStreamInput.h"
#include "IByteWriterWithPosition.h"

#include <iostream>
#include <set>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

int PDFParserInvalidLengthTest(int argc, char* argv[])
{
	EStatusCode status = PDFHummus::eSuccess;
	InputFile pdfFile;
	PDFParser parser;

	do
	{
		status = pdfFile.OpenFile(BuildRelativeInputPath(argv, "InvalidEncryptionLength.pdf"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to open file for reading. should be in TestMaterials/XObjectContent.pdf\n";
			break;
		}

		status = parser.StartPDFParsing(pdfFile.GetInputStream());
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to parse input file";
			break;
		}
	}while(false);

	return status;
}


