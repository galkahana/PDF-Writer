#pragma once

#include "EStatusCode.h"
#include "IByteWriterWithPosition.h"

#include <string>
#include <stdio.h>

using namespace std;


class OutputFileStream : public IByteWriterWithPosition
{
public:
	OutputFileStream(void);
	virtual ~OutputFileStream(void);

	OutputFileStream(const wstring& inFilePath,bool inAppend = false);

	EStatusCode Open(const wstring& inFilePath,bool inAppend = false);
	EStatusCode Close();

	// IByteWriter implementation
	virtual LongBufferSizeType Write(const Byte* inBuffer,LongBufferSizeType inSize);

	// IByteWriterWithPosition implementation
	virtual LongFilePositionType GetCurrentPosition();

private:

	FILE* mStream;
};
