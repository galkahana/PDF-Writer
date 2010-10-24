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

	/*
		Skip inSkipSize bytes. if not enough stop at EOF.
	*/
	virtual void Skip(LongBufferSizeType inSkipSize) = 0;

	/*
		Set read position.always set from start
	*/
	virtual void SetPosition(LongFilePositionType inOffsetFromStart) = 0;

	/*
		Get the current read position
	*/
	virtual LongFilePositionType GetCurrentPosition() = 0;

};
