/*
   Source File : Trace.cpp


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
#include "Trace.h"
#include "Log.h"
#include "SafeBufferMacrosDefs.h"

#include <stdio.h>
#include <stdarg.h>


Trace::Trace(void)
{
	mLog = NULL;
	mLogFilePath = L"Log.txt";
	mShouldLog = false;
}

Trace::~Trace(void)
{
	delete mLog;
}

void Trace::SetLogSettings(const wstring& inLogFilePath,bool inShouldLog)
{
	mShouldLog = inShouldLog;
	mLogFilePath = inLogFilePath;
	if(mLog != NULL)
	{
		delete mLog;
		mLog = NULL;
		if(mShouldLog)
			mLog = new Log(mLogFilePath);
	}
}

void Trace::TraceToLog(const wchar_t* inFormat,...)
{
	if(mShouldLog)
	{
		if(NULL == mLog)
			mLog = new Log(mLogFilePath);

		va_list argptr;
		va_start(argptr, inFormat);

		SAFE_VSWPRINTF(mBuffer,5001,inFormat,argptr);
		va_end(argptr);

		mLog->LogEntry(wstring(mBuffer));
	}
}

void Trace::TraceToLog(const wchar_t* inFormat,va_list inList)
{
	if(mShouldLog)
	{
		if(NULL == mLog)
			mLog = new Log(mLogFilePath);

		SAFE_VSWPRINTF(mBuffer,5001,inFormat,inList);

		mLog->LogEntry(wstring(mBuffer));
	}

}

