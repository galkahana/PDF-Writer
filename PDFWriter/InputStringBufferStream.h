#pragma once

#include "IByteReader.h"
#include "MyStringBuf.h"

#include <sstream>
#include <string>

using namespace std;

class InputStringBufferStream : public IByteReader
{
public:
	InputStringBufferStream(MyStringBuf* inBufferToReadFrom);
	~InputStringBufferStream(void);

	void Assign(MyStringBuf* inBufferToReadFrom);

	// IByteReader implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);
	virtual LongFilePositionType GetCurrentPosition();

private:
	MyStringBuf* mBufferToReadFrom;


};
