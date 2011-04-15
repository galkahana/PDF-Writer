/*
   Source File : PDFTextStringTest.cpp


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
#include "PDFTextStringTest.h"
#include "PDFTextString.h"

#include <iostream>

using namespace std;

PDFTextStringTest::PDFTextStringTest(void)
{
}

PDFTextStringTest::~PDFTextStringTest(void)
{
}

EStatusCode PDFTextStringTest::Run()
{
	EStatusCode status = eSuccess;

	// Empty String test
	PDFTextString emptyString;
	if(emptyString.ToString() != "")
	{
		status = eFailure;
		wcout<<"wrong for empty string - "<<emptyString.ToString().c_str()<<"\n";
	}

	// PDFEncoded test, Latin
	PDFTextString latinString(L"Hello World");
	if(latinString.ToString() != "Hello World")
	{
		status = eFailure;
		wcout<<"wrong for empty string - "<<latinString.ToString().c_str()<<"\n";
	}

	// PDFEncoded test, special char
	wstring aString;
	aString.push_back(0x20AC);
	PDFTextString latinSpecialString(aString);
	if(latinSpecialString.ToString() != "\xA0")
	{
		status = eFailure;
		wcout<<"wrong for latin special string - "<<latinSpecialString.ToString().c_str()<<"\n";
	}

	// UTF16 test
	wstring bString;
	bString.push_back(0x20AB);
	PDFTextString latinUTF16String(bString);
	if(latinUTF16String.ToString() != "\xFE\xFF\x20\xAB")
	{
		status = eFailure;
		wcout<<"wrong for latin UTF16 - "<<latinUTF16String.ToString().c_str()<<"\n";
	}

	return status;
}


ADD_CATEGORIZED_TEST(PDFTextStringTest,"ObjectContext")
