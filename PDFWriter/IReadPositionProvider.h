#pragma once

#include "IOBasicTypes.h"

class IReadPositionProvider
{
public:

	virtual ~IReadPositionProvider(){}

	/*
		Get the current read position
	*/
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition() = 0;

};