#pragma once
/*
	IByteReaderWithPositionWithPosition. extension of byte reader with position setting and getting
*/

#include "IByteReader.h"

class IByteReaderWithPosition : public IByteReader
{
public:
	virtual ~IByteReaderWithPosition(void){};

	/*
		Set read position.always set from start
	*/
	virtual void SetPosition(LongFilePositionType inOffsetFromStart) = 0;

	/*
		Get the current read position
	*/
	virtual LongFilePositionType GetCurrentPosition() = 0;

	/*
		skip position (like setting from current)
	*/
	virtual void Skip(LongBufferSizeType inSkipSize) = 0;


};
