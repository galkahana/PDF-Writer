#pragma once
/*
	IByteWriterWithPosition.
	extension of IByteWriter that also allows for retrieving the current position

*/

#include "IByteWriter.h"

class IByteWriterWithPosition : public IByteWriter
{
public:
	virtual ~IByteWriterWithPosition(void){};

	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition() = 0;
};
