#pragma once

#include "BoxingBase.h"

class PSBoolReader
{
public:
	void Read(const string& inReadFrom,bool& outValue);
	void Read(const wstring& inReadFrom,bool& outValue);
};

void PSBoolReader::Read(const string& inReadFrom,bool& outValue)
{
	if(inReadFrom.compare("true") == 0)
		outValue = true;
	else
		outValue = false;
}

void PSBoolReader::Read(const wstring& inReadFrom,bool& outValue)
{
	if(inReadFrom.compare(L"true") == 0)
		outValue = true;
	else
		outValue = false;
}

class PSBoolWriter
{
public:
	void Write(const bool& inValue,string& outWriteTo);
	void Write(const bool& inValue,wstring& outWriteTo);
};

void PSBoolWriter::Write(const bool& inValue,string& outWriteTo)
{
	if(inValue)
		outWriteTo = "true";
	else
		outWriteTo = "false";
}

void PSBoolWriter::Write(const bool& inValue,wstring& outWriteTo)
{
	if(inValue)
		outWriteTo = L"true";
	else
		outWriteTo = L"false";
}

typedef BoxingBaseWithRW<bool,PSBoolReader,PSBoolWriter> PSBool;

