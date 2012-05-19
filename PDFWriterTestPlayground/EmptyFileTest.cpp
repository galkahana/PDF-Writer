/*
   Source File : EmptyFileTest.cpp


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
#include "EmptyFileTest.h"
#include "PDFWriter.h"


#include <iostream>

using namespace std;
using namespace PDFHummus;


EmptyFileTest::EmptyFileTest(void)
{
}

EmptyFileTest::~EmptyFileTest(void)
{
}

EStatusCode EmptyFileTest::Run(const TestConfiguration& inTestConfiguration)
{
	PDFWriter pdfWriter;
	LogConfiguration logConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"EmptyFileLog.txt"));
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"test.txt"),ePDFVersion13,logConfiguration);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	
		
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;
}

ADD_CATEGORIZED_TEST(EmptyFileTest,"PDF")
