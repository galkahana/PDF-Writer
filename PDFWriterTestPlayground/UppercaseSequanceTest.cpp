/*
   Source File : UppercaseSequanceTest.cpp


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
#include "UppercaseSequanceTest.h"
#include "UppercaseSequance.h"
#include <iostream>

using namespace std;
using namespace PDFHummus;

UppercaseSequanceTest::UppercaseSequanceTest(void)
{
}

UppercaseSequanceTest::~UppercaseSequanceTest(void)
{
}

EStatusCode UppercaseSequanceTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;
	UppercaseSequance sequance;

	do
	{
		string value = sequance.GetNextValue();
		if(value != "AAAAAB")
		{
			cout<<"Failed in single increase. value is  "<<value.c_str()<<". should be AAAAAB\n";
			status = PDFHummus::eFailure;
			break;
		}
		else
		{
			cout<<"Success in single increase\n";
		}
	
		for(int i=0;i<25;++i)
			value = sequance.GetNextValue();
		if(value != "AAAABA")
		{
			cout<<"Failed in 24 increase. value is  "<<value.c_str()<<". should be AAAABA\n";
			status = PDFHummus::eFailure;
			break;
		}
		else
		{
			cout<<"Success in 24 increase\n";
		}

		for(int j=0;j < 26*25; ++j)
			value = sequance.GetNextValue();
		if(value != "AAABAA")
		{
			cout<<"Failed in 26*25 increase. value is  "<<value.c_str()<<". should be AAABAA\n";
			status = PDFHummus::eFailure;
			break;
		}
		else
		{
			cout<<"Success in 26*25 increase\n";
		}
	
	}while(false);

	return status;
}


ADD_CATEGORIZED_TEST(UppercaseSequanceTest,"ObjectContext")
