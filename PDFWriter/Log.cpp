/*
   Source File : Log.cpp


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
#include "Log.h"
#include "SafeBufferMacrosDefs.h"
#include "IByteWriterWithPosition.h"
#include <ctime>
#include <stdio.h>

Log::Log(const wstring& inLogFilePath)
{
	mFilePath = inLogFilePath;

	// check if file exists or not...if not, create new one and place a bom in its beginning
	FILE* logFile;
	bool exists;
	SAFE_WFOPEN(logFile,mFilePath.c_str(),L"r")
	exists = (logFile!= NULL);
	if(logFile)
		fclose(logFile);

	if(!exists)
	{
		// let's trick the system to tell us which bom it uses..
		unsigned short bom = (0xFE<<8) + 0xFF;

		// put unicode header
		mLogFile.OpenFile(mFilePath);
		mLogFile.GetOutputStream()->Write((const Byte*)&bom,2);	
		mLogFile.CloseFile();
	}
}

Log::~Log(void)
{
}

void Log::LogEntry(const wstring& inMessage)
{
	LogEntry((const Byte*)inMessage.c_str(), inMessage.length() * 2);
}

static const wchar_t scEndLine[] = L"\r\n";
void Log::LogEntry(const Byte* inMessage, LongBufferSizeType inMessageSize)
{
	mLogFile.OpenFile(mFilePath,true);
	wstring formattedTimeString = GetFormattedTimeString();
	mLogFile.GetOutputStream()->Write((const Byte*)formattedTimeString.c_str(),formattedTimeString.length()*2);
	mLogFile.GetOutputStream()->Write(inMessage,inMessageSize);
	mLogFile.GetOutputStream()->Write((const Byte*)scEndLine,4);
	mLogFile.CloseFile();
}

wstring Log::GetFormattedTimeString()
{
	// create a local time string (date + time) that looks like this: "[ dd/mm/yyyy hh:mm:ss ] "
	wchar_t buffer[26];
	
	time_t currentTime;
	tm structuredLocalTime;

	time(&currentTime);
	SAFE_LOCAL_TIME(structuredLocalTime,currentTime);

	SAFE_SWPRINTF_6(buffer,26,L"[ %02d/%02d/%04d %02d:%02d:%02d ] ",structuredLocalTime.tm_mday,
																	structuredLocalTime.tm_mon + 1,
																	structuredLocalTime.tm_year + 1900,
																	structuredLocalTime.tm_hour,
																	structuredLocalTime.tm_min,
																	structuredLocalTime.tm_sec);
	return buffer;
}
