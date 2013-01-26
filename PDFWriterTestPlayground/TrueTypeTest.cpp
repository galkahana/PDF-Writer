/*
   Source File : TrueTypeTest.cpp


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
#include "TrueTypeTest.h"
#include "OpenTypeFileInput.h"
#include "InputFile.h"
#include "TestsRunner.h"

#include <iostream>

using namespace PDFHummus;

TrueTypeTest::TrueTypeTest(void)
{
}

TrueTypeTest::~TrueTypeTest(void)
{
}

EStatusCode TrueTypeTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;
	InputFile ttfFile;

	do
	{
		status = ttfFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/fonts/arial.ttf"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"cannot read arial font file\n";
			break;
		}

		OpenTypeFileInput trueTypeReader;

		status = trueTypeReader.ReadOpenTypeFile(ttfFile.GetInputStream(),0);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"could not read true type file\n";
			break;
		}
	}while(false);

	return status;
}

ADD_CATEGORIZED_TEST(TrueTypeTest,"TrueType")
