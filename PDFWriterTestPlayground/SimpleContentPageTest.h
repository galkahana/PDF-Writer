#pragma once
#include "ITestUnit.h"

class SimpleContentPageTest : public ITestUnit
{
public:
	SimpleContentPageTest(void);
	~SimpleContentPageTest(void);

	virtual EStatusCode Run();
};
