#pragma once

#include "EStatusCode.h"
#include "IByteReaderWithPosition.h"

#include <string>
#include <stdio.h>

using namespace std;


class InputFileStream : public IByteReaderWithPosition
{
public:
	InputFileStream(void);
	virtual ~InputFileStream(void);

	InputFileStream(const wstring& inFilePath);

	EStatusCode Open(const wstring& inFilePath);
	EStatusCode Close();

	// IByteReaderWithPosition implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);
	virtual LongFilePositionType GetCurrentPosition();

	LongFilePositionType GetFileSize();

private:

	FILE* mStream;
};
