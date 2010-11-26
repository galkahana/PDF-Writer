#pragma once
#include "ITestUnit.h"

class CFFFileInput;

class SimpleTextUsage : public ITestUnit
{
public:
	SimpleTextUsage(void);
	virtual ~SimpleTextUsage(void);

	virtual EStatusCode Run();

private:

	EStatusCode RunCFFTest();
	EStatusCode RunTrueTypeTest();
	EStatusCode RunType1Test();
};
