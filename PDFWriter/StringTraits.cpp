#include "StringTraits.h"

#include <sstream>

StringTraits::StringTraits(const string& inString):mString(inString)
{
}

StringTraits::~StringTraits(void)
{
}

wstring StringTraits::WidenString()
{
	wstringstream formatter;
	string::const_iterator it=mString.begin();
	wchar_t aWideChar;

	for(;it != mString.end();++it)
	{
		aWideChar = (wchar_t)*it;
		formatter.put(aWideChar);
	}
	return formatter.str();
	
}

StringTraits::operator const string&()
{
	return mString;
}
