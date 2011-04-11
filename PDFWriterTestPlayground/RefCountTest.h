#pragma once

#include "TestsRunner.h"

class RefCountTest : public ITestUnit
{
public:
	RefCountTest(void);
	virtual ~RefCountTest(void);

	virtual EStatusCode Run();
};
