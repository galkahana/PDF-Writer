#pragma once
#include "TestsRunner.h"

class UppercaseSequanceTest: public ITestUnit
{
public:
	UppercaseSequanceTest(void);
	~UppercaseSequanceTest(void);

	virtual EStatusCode Run();
};
