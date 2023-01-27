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
#include "BoxingBase.h"

#include <iostream>

using namespace std;

int BoxingBaseTest(int argc, char* argv[]) {
	int status = 0;

	do {
		// Assignment
		Int a = 3;
		a = 2;
		if((int)a != 2)
		{
			cout<<"Assignment failed\n";
			status = 1;
			break;
		}

		// Initialization
		Int b(2);
		if((int)b != 2)
		{
			cout<<"Initialization failed\n";
			status = 1;
			break;
		}


		// From string
		Int c("2");
		if((int)c != 2)
		{
			cout<<"Initialization from string failed\n";
			status = 1;
			break;
		}

		// From wide string
		Int d(L"2");
		if((int)d != 2)
		{
			cout<<"Initialization from wide string failed\n";
			status = 1;
			break;
		}

		// string write
		Int e(2);
		if(e.ToString() != "2")
		{
			cout<<"Write to string failed\n";
			status = 1;
			break;
		}

		// wide string write
		Int f(2);
		if(f.ToWString() != L"2")
		{
			cout<<"Write to wide string failed\n";
			status = 1;
			break;
		}
	} while(false);

	return status;
}
