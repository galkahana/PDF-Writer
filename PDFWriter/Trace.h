/*
   Source File : Trace.h


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
#pragma once
#include "singleton.h"

#include <string>

using namespace std;

// good for tracing upto 5K wide chars messages

class Log;
class IByteWriter;

class Trace
{
public:
	Trace();
	~Trace(void);

	void SetLogSettings(const wstring& inLogFilePath,bool inShouldLog);
	void SetLogSettings(IByteWriter* inLogStream,bool inShouldLog);

	void TraceToLog(const wchar_t* inFormat,...);
	void TraceToLog(const wchar_t* inFormat,va_list inList);


private:
	wchar_t mBuffer[5001];
	Log* mLog;

	wstring mLogFilePath;
	IByteWriter* mLogStream;
	bool mShouldLog;

	// use the log
	// make it lock safe
};


// wide log. remember to place a nice li'l "L" before each string
#define TRACE_WLOG(FORMAT) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT)
#define TRACE_WLOG1(FORMAT,ARG1) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1)
#define TRACE_WLOG2(FORMAT,ARG1,ARG2) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2)
#define TRACE_WLOG3(FORMAT,ARG1,ARG2,ARG3) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2,ARG3)
#define TRACE_WLOG4(FORMAT,ARG1,ARG2,ARG3,ARG4) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2,ARG3,ARG4)
#define TRACE_WLOG5(FORMAT,ARG1,ARG2,ARG3,ARG4,ARG5) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2,ARG3,ARG4,ARG5)

// short cuts for logging formats strings without having to concat the "L". args still need to be taken care of
#define TRACE_LOG(FORMAT) Singleton<Trace>::GetInstance()->TraceToLog(L##FORMAT)
#define TRACE_LOG1(FORMAT,ARG1) Singleton<Trace>::GetInstance()->TraceToLog(L##FORMAT,ARG1)
#define TRACE_LOG2(FORMAT,ARG1,ARG2) Singleton<Trace>::GetInstance()->TraceToLog(L##FORMAT,ARG1,ARG2)
#define TRACE_LOG3(FORMAT,ARG1,ARG2,ARG3) Singleton<Trace>::GetInstance()->TraceToLog(L##FORMAT,ARG1,ARG2,ARG3)
#define TRACE_LOG4(FORMAT,ARG1,ARG2,ARG3,ARG4) Singleton<Trace>::GetInstance()->TraceToLog(L##FORMAT,ARG1,ARG2,ARG3,ARG4)
#define TRACE_LOG5(FORMAT,ARG1,ARG2,ARG3,ARG4,ARG5) Singleton<Trace>::GetInstance()->TraceToLog(L##FORMAT,ARG1,ARG2,ARG3,ARG4,ARG5)



