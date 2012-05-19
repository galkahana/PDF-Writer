/*
   Source File : PDFDateTest.cpp


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
#include "PDFDateTest.h"


#include <iostream>

using namespace std;
using namespace PDFHummus;

PDFDateTest::PDFDateTest(void)
{
}

PDFDateTest::~PDFDateTest(void)
{
}

EStatusCode PDFDateTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;

	// Empty Date
	PDFDate emptyDate;
	if(emptyDate.ToString() != "")
	{
		status = PDFHummus::eFailure;
		cout<<"wrong string conversion for empty date - "<<emptyDate.ToString().c_str()<<"\n";
	}

	// Year only Date
	PDFDate yearDate;
	yearDate.Year = 1984;
	if(yearDate.ToString() != "D:1984")
	{
		status = PDFHummus::eFailure;
		cout<<"wrong string conversion for year date - "<<yearDate.ToString().c_str()<<"\n";
	}

	// Day only Date
	PDFDate dayDate;
	dayDate.Year = 1984;
	dayDate.Month = 4;
	dayDate.Day = 3;
	if(dayDate.ToString() != "D:19840403")
	{
		status = PDFHummus::eFailure;
		cout<<"wrong string conversion for day date - "<<dayDate.ToString().c_str()<<"\n";
	}
	// local Time Date
	PDFDate localTimeDate;
	localTimeDate.Year = 1984;
	localTimeDate.Month = 4;
	localTimeDate.Day = 3;
	localTimeDate.Hour = 18;
	localTimeDate.Minute = 30;
	localTimeDate.Second = 45;
	if(localTimeDate.ToString() != "D:19840403183045")
	{
		status = PDFHummus::eFailure;
		cout<<"wrong string conversion for local date - "<<localTimeDate.ToString().c_str()<<"\n";
	}
	// Fully qualified Time date
	PDFDate fullDate;
	fullDate.Year = 1984;
	fullDate.Month = 4;
	fullDate.Day = 3;
	fullDate.Hour = 18;
	fullDate.Minute = 30;
	fullDate.Second = 45;
	fullDate.UTC = PDFDate::eEarlier;
	fullDate.HourFromUTC = 2;
	fullDate.MinuteFromUTC = 0;
	if(fullDate.ToString() != "D:19840403183045-02'00'")
	{
		status = PDFHummus::eFailure;
		cout<<"wrong string conversion for full date - "<<fullDate.ToString().c_str()<<"\n";
	}

	// Just display the current time as set by PDFDate
	PDFDate currentDate;
	currentDate.SetToCurrentTime();
	cout<<"Current time as represented in PDFDate - "<<currentDate.ToString().c_str()<<"\n";

	return status;
}


ADD_CATEGORIZED_TEST(PDFDateTest,"ObjectContext")
