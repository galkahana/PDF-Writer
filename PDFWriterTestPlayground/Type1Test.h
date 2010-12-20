#pragma once
#include "ITestUnit.h"
#include <string>

using namespace std;

class Type1Input;

class Type1Test : public ITestUnit
{
public:
	Type1Test(void);
	~Type1Test(void);

	virtual EStatusCode Run();

private:
	EStatusCode SaveCharstringCode(const string& inCharStringName,Type1Input* inType1Input);
	EStatusCode ShowDependencies(const string& inCharStringName,Type1Input* inType1Input);

};
