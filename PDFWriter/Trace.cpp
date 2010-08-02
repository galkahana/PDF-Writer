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

