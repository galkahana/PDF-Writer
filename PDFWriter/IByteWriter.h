#pragma once
/*
	IByteWriter. interface for writing bytes


*/
#include "IOBasicTypes.h"

class IByteWriter
{
public:
	virtual ~IByteWriter(void){};

	/*
		Write inSize bytes of inBuffer, returning the number of written bytes
	*/
	virtual IOBasicTypes::LongBufferSizeType Write(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize) = 0;
};
