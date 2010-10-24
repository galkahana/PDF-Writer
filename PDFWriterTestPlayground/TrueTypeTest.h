#pragma once
#include "ITestUnit.h"

class TrueTypeTest : public ITestUnit
{
public:
	TrueTypeTest(void);
	~TrueTypeTest(void);

	virtual EStatusCode Run();
};
