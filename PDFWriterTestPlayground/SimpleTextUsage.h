#pragma once
#include "ITestUnit.h"

class SimpleTextUsage : public ITestUnit
{
public:
	SimpleTextUsage(void);
	virtual ~SimpleTextUsage(void);

	virtual EStatusCode Run();
};
