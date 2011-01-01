#pragma once
#include "ITestUnit.h"

class MyTest : public ITestUnit
{
public:
	MyTest(void);
	~MyTest(void);

	virtual EStatusCode Run();

};
