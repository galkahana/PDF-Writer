#pragma once


#include <list>
#include <map>

using namespace std;

struct DictOperand
{
	bool IsInteger;
	union
	{
		long IntegerValue;
		double RealValue;
	};
	long RealValueFractalEnd; // this fellow is here for writing, due to double being terribly inexact.
};

typedef list<DictOperand> DictOperandList;

// that would actually be a dictionary
typedef map<unsigned short,DictOperandList> UShortToDictOperandListMap;
