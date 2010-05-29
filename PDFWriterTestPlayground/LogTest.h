#pragma once

#include "TestsRunner.h"

class LogTest: public ITestUnit
{
public:
	LogTest(void);
	~LogTest(void);

	virtual EStatusCode Run();
};
