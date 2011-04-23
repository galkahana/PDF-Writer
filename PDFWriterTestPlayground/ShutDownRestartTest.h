#pragma once
#include "ITestUnit.h"

class ShutDownRestartTest : public ITestUnit
{
public:
	ShutDownRestartTest(void);
	virtual ~ShutDownRestartTest(void);

	virtual EStatusCode Run();
};
