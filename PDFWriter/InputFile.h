#pragma once

#include "EStatusCode.h"
#include <string>

class IByteReader;
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
private:
	wstring mFilePath;
	InputBufferedStream* mInputStream;
	InputFileStream* mFileStream;
};
