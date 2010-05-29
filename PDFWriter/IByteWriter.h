#pragma once
/*
	IByteWriter. interface for writing bytes


*/
#include "IOBasicTypes.h"

using namespace IOBasicTypes;

class IByteWriter
{
public:
	virtual ~IByteWriter(void){};

	/*
		Write inSize bytes of inBuffer, returning the number of written bytes
	*/
	virtual LongBufferSizeType Write(const Byte* inBuffer,LongBufferSizeType inSize) = 0;
};
