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

	void SetLogSettings(const string& inLogFilePath,bool inShouldLog,bool inPlaceUTF8Bom);
	void SetLogSettings(IByteWriter* inLogStream,bool inShouldLog);

	void TraceToLog(const char* inFormat,...);
	void TraceToLog(const char* inFormat,va_list inList);


private:
	char mBuffer[5001];
	Log* mLog;

	string mLogFilePath;
	IByteWriter* mLogStream;
	bool mShouldLog;
	bool mPlaceUTF8Bom;

	// use the log
	// make it lock safe
};


// short cuts for logging formats strings
#define TRACE_LOG(FORMAT) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT)
#define TRACE_LOG1(FORMAT,ARG1) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1)
#define TRACE_LOG2(FORMAT,ARG1,ARG2) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2)
#define TRACE_LOG3(FORMAT,ARG1,ARG2,ARG3) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2,ARG3)
#define TRACE_LOG4(FORMAT,ARG1,ARG2,ARG3,ARG4) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2,ARG3,ARG4)
#define TRACE_LOG5(FORMAT,ARG1,ARG2,ARG3,ARG4,ARG5) Singleton<Trace>::GetInstance()->TraceToLog(FORMAT,ARG1,ARG2,ARG3,ARG4,ARG5)



