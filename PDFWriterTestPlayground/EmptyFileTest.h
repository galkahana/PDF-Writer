#pragma once
#include "TestsRunner.h"

class EmptyFileTest : public ITestUnit
{
public:
	EmptyFileTest(void);
	~EmptyFileTest(void);

	virtual EStatusCode Run();
};