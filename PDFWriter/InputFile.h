#pragma once

#include "EStatusCode.h"
#include "IByteReader.h"
#include <string>

class InputBufferedStream;
class InputFileStream;

using namespace std;

class InputFile
{
public:
	InputFile(void);
	~InputFile(void);

	EStatusCode OpenFile(const wstring& inFilePath);
	EStatusCode CloseFile();

	IByteReader* GetInputStream(); // returns buffered input stream
	const wstring& GetFilePath();
	
	LongFilePositionType GetFileSize();

private:
	wstring mFilePath;
	InputBufferedStream* mInputStream;
	InputFileStream* mFileStream;
};
