#pragma once

#include <string>
using namespace std;

class UppercaseSequance
{
public:
	UppercaseSequance(void);
	~UppercaseSequance(void);

	// returns the string after the increase
	const string& GetNextValue();

private:
	string mSequanceString;

};
