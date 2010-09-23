#pragma once

#include "EStatusCode.h"
#include "IByteReader.h"

#include <string>
#include <stdio.h>

using namespace std;


class InputFileStream : public IByteReader
{
public:
	InputFileStream(void);
	virtual ~InputFileStream(void);

	InputFileStream(const wstring& inFilePath);

	EStatusCode Open(const wstring& inFilePath);
	EStatusCode Close();

	// IByteReader implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);

	LongFilePositionType GetFileSize();

private:

	FILE* mStream;
};
