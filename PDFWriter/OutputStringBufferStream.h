#pragma once

#include "IByteWriterWithPosition.h"
#include <sstream>
#include <string>

using namespace std;

class OutputStringBufferStream : public IByteWriterWithPosition
{
public:
	OutputStringBufferStream(void);
	~OutputStringBufferStream(void);

	// override for having the stream control an external buffer. NOT taking ownership
	OutputStringBufferStream(stringbuf* inControlledBuffer);


	// IByteWriter implementation
	virtual IOBasicTypes::LongBufferSizeType Write(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize);

	// IByteWriterWithPosition implementation
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition();

	string ToString() const;

	void Reset();

private:
	stringbuf* mBuffer;
	bool mOwnsBuffer;

};
