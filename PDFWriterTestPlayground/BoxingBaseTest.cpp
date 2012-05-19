/*
   Source File : BoxingBaseTest.cpp


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
#include "BoxingBaseTest.h"
#include "TestsRunner.h"
#include "BoxingBase.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

BoxingBaseTest::BoxingBaseTest(void)
{
}

BoxingBaseTest::~BoxingBaseTest(void)
{
}

EStatusCode BoxingBaseTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = RunIntTest();

	return status;
}

EStatusCode BoxingBaseTest::RunIntTest()
{
	EStatusCode status = PDFHummus::eSuccess;

		
	// Assignment
	Int a = 3;
	a = 2;
	if((int)a != 2)
	{
		status = PDFHummus::eFailure;
		cout<<"Assignment failed\n";
	}

	// Initialization
	Int b(2);
	if((int)b != 2)
	{
		status = PDFHummus::eFailure;
		cout<<"Initialization failed\n";
	}


	// From string
	Int c("2");
	if((int)c != 2)
	{
		status = PDFHummus::eFailure;
		cout<<"Initialization from string failed\n";
	}

	// From wide string
	Int d("2");
	if((int)d != 2)
	{
		status = PDFHummus::eFailure;
		cout<<"Initialization from wide string failed\n";
	}

	// string write
	Int e(2);
	if(e.ToString() != "2")
	{
		status = PDFHummus::eFailure;
		cout<<"Write to string failed\n";
	}

	// wide string write
	Int f(2);
	if(f.ToWString() != L"2")
	{
		status = PDFHummus::eFailure;
		cout<<"Write to wide string failed\n";
	}


	return status;
}

ADD_CATEGORIZED_TEST(BoxingBaseTest,"Patterns")
