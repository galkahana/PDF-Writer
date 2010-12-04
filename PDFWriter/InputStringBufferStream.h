#pragma once

#include "IByteReaderWithPosition.h"
#include "MyStringBuf.h"

#include <sstream>
#include <string>

using namespace std;

class InputStringBufferStream : public IByteReaderWithPosition
{
public:
	InputStringBufferStream(MyStringBuf* inBufferToReadFrom);
	~InputStringBufferStream(void);

	void Assign(MyStringBuf* inBufferToReadFrom);

	// IByteReaderWithPosition implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);
	virtual LongFilePositionType GetCurrentPosition();

private:
	MyStringBuf* mBufferToReadFrom;


};
