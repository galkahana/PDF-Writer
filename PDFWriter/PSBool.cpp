#include "PSBool.h"

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