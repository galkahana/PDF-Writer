#pragma once

#include "IByteReader.h"

class InputLimitedStream : public IByteReader
{
public:
	InputLimitedStream();
	virtual ~InputLimitedStream(void);

	// K. with this one (just cause i'm tired of this ownership thing) i'm trying something new - to flag ownership. 
	// So make good use of this last flag
	InputLimitedStream(IByteReader* inSourceStream,IOBasicTypes::LongFilePositionType inReadLimit,bool inOwnsStream);

	void Assign(IByteReader* inSourceStream,IOBasicTypes::LongFilePositionType inReadLimit,bool inOwnsStream);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);
	virtual bool NotEnded();

private:

	bool mOwnsStream;
	IByteReader* mStream;
	IOBasicTypes::LongFilePositionType mMoreToRead;
};
