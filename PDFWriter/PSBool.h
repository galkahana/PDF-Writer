#pragma once

#include "BoxingBase.h"

class PSBoolReader
{
public:
	void Read(const string& inReadFrom,bool& outValue);
	void Read(const wstring& inReadFrom,bool& outValue);
};

class PSBoolWriter
{
public:
	void Write(const bool& inValue,string& outWriteTo);
	void Write(const bool& inValue,wstring& outWriteTo);
};

typedef BoxingBaseWithRW<bool,PSBoolReader,PSBoolWriter> PSBool;

