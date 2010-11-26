#pragma once

#include "IOBasicTypes.h"
#include <list>

using namespace IOBasicTypes;
using namespace std;

struct IndexElement
{
	IndexElement() {mStartPosition=0; mEndPosition=0; mIndex=0;}

	LongFilePositionType mStartPosition;
	LongFilePositionType mEndPosition;
	unsigned short mIndex;
};

typedef IndexElement CharString;

struct CharStringOperand
{
	bool IsInteger;
	union
	{
		long IntegerValue;
		double RealValue;
	};
};

typedef list<CharStringOperand> CharStringOperandList;
