#pragma once

#include "IByteReaderWithPosition.h"

class InputByteArrayStream : public IByteReaderWithPosition
{
public:
	InputByteArrayStream();
	InputByteArrayStream(Byte* inByteArray,LongFilePositionType inArrayLength);
	~InputByteArrayStream(void);

	void Assign(Byte* inByteArray,LongFilePositionType inArrayLength);

	// IByteReaderWithPosition implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);
	virtual LongFilePositionType GetCurrentPosition();

private:

	Byte* mByteArray;
	LongFilePositionType mArrayLength;
	LongFilePositionType mCurrentPosition;

};
