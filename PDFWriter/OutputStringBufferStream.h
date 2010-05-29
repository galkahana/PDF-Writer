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
	virtual LongBufferSizeType Write(const Byte* inBuffer,LongBufferSizeType inSize);

	// IByteWriterWithPosition implementation
	virtual LongFilePositionType GetCurrentPosition();

	string ToString() const;

	void Reset();

private:
	stringbuf* mBuffer;
	bool mOwnsBuffer;

};
