/*
   Source File : LogTest.cpp


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
#include "LogTest.h"
#include "Log.h"
#include "Trace.h"

using namespace PDFHummus;

LogTest::LogTest(void)
{
}

LogTest::~LogTest(void)
{
}

EStatusCode LogTest::Run(const TestConfiguration& inTestConfiguration)
{
	Log log(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"logTest.txt"),true);

	log.LogEntry("testing wide string input");

	const char* aString = "testing Byte input";

	log.LogEntry((const Byte*)aString,strlen(aString));


	Trace trace;

	trace.SetLogSettings(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"traceTest.txt"),true,true);
	trace.TraceToLog("Tracing number %d %d",10,20);
	trace.TraceToLog("Tracing some other items %s 0x%x","hello",20);

	return PDFHummus::eSuccess;
}

ADD_CATEGORIZED_TEST(LogTest,"IO")
