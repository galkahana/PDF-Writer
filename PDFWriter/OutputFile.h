#pragma once

#include "EStatusCode.h"
#include <string>

class IByteWriterWithPosition;
class BufferedOutputStream;
class OutputFileStream;

using namespace std;

class OutputFile
{
public:
	OutputFile(void);
	~OutputFile(void);

	EStatusCode OpenFile(const wstring& inFilePath, bool inAppend = false);
	EStatusCode CloseFile();

	IByteWriterWithPosition* GetOutputStream(); // returns buffered output stream
	const wstring& GetFilePath();
private:
	wstring mFilePath;
	BufferedOutputStream* mOutputStream;
	OutputFileStream* mFileStream;
};
