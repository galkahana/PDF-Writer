#pragma once
/*
	IByteReader. interface for reading bytes


*/

#include "IOBasicTypes.h"

using namespace IOBasicTypes;

class IByteReader 
{
public:
	virtual ~IByteReader(void){};

	/*
		Read inSize bytes of inBuffer, returning the number of bytes actually read
	*/
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize) = 0;

	/*
		boolean to determine whether reading will get more content or not. opposite of IsEOF. cause i'm sick of !IsEOF()s
	*/
	virtual bool NotEnded() = 0;

};
