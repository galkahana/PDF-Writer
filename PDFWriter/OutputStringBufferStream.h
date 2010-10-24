#pragma once

#include "IByteWriterWithPosition.h"
#include "MyStringBuf.h"
#include <string>

using namespace std;

class OutputStringBufferStream : public IByteWriterWithPosition
{
public:
	OutputStringBufferStream(void);
	~OutputStringBufferStream(void);

	// override for having the stream control an external buffer. NOT taking ownership
	OutputStringBufferStream(MyStringBuf* inControlledBuffer);

	void Assign(MyStringBuf* inControlledBuffer); // can assign a new one after creation


	// IByteWriter implementation
	virtual IOBasicTypes::LongBufferSizeType Write(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize);

	// IByteWriterWithPosition implementation
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition();

	string ToString() const;
	void Reset();
	void SetPosition(IOBasicTypes::LongFilePositionType inOffsetFromStart);


private:
	MyStringBuf* mBuffer;
	bool mOwnsBuffer;

};
