#pragma once

#include "IByteReader.h"

class InputLimitedStream : public IByteReader
{
public:
	InputLimitedStream();
	virtual ~InputLimitedStream(void);

	// Owns inSourceStream, so use Assign(NULL,0) if when done if this is not the case
	InputLimitedStream(IByteReader* inSourceStream,IOBasicTypes::LongFilePositionType inReadLimit);

	// Owns inSourceStream, so use Assign(NULL,0) if when done if this is not the case
	void Assign(IByteReader* inSourceStream,IOBasicTypes::LongFilePositionType inReadLimit);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);
	virtual bool NotEnded();

private:

	IByteReader* mStream;
	IOBasicTypes::LongFilePositionType mMoreToRead;
};
