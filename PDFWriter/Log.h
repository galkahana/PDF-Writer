#pragma once

#include "IOBasicTypes.h"
#include "OutputFile.h"

#include <string>

using namespace std;
using namespace IOBasicTypes;

class Log
{
public:
	Log(const wstring& inLogFilePath);
	~Log(void);

	void LogEntry(const wstring& inMessage);
	void LogEntry(const Byte* inMessage, LongBufferSizeType inMessageSize);

private:

	wstring mFilePath;
	OutputFile mLogFile;

	wstring GetFormattedTimeString();

};
