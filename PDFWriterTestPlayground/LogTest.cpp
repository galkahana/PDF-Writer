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

LogTest::LogTest(void)
{
}

LogTest::~LogTest(void)
{
}

EStatusCode LogTest::Run()
{
	Log log(L"C:\\PDFLibTests\\logTest.txt");

	log.LogEntry(L"testing wide string input");

	wchar_t* aWideString = L"testing Byte input";

	log.LogEntry((const Byte*)aWideString,wcslen(aWideString)*2);


	Trace trace;

	trace.SetLogSettings(L"C:\\PDFLibTests\\traceTest.txt",false);
	trace.TraceToLog(L"Tracing number %d %d",10,20);
	trace.TraceToLog(L"Tracing some other items %s 0x%x",L"hello",20);

	return eSuccess;
}

ADD_CETEGORIZED_TEST(LogTest,"IO")