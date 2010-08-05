#pragma once

#include "TestsRunner.h"

class TimerTest : public ITestUnit
{
public:
	TimerTest(void);
	~TimerTest(void);

	virtual EStatusCode Run();

private:
	void Sleep(double inSeconds);
};
