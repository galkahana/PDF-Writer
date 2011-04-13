#pragma once
#include "ITestUnit.h"

class AppendPagesTest : public ITestUnit
{
public:
	AppendPagesTest(void);
	virtual ~AppendPagesTest(void);

	virtual EStatusCode Run();
};
