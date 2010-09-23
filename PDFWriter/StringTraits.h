#pragma once

#include <string>

using namespace std;

class StringTraits
{
public:
	StringTraits(const string& inString);
	~StringTraits(void);

	wstring WidenString();

	operator const string&();

private:
	const string& mString;
};
