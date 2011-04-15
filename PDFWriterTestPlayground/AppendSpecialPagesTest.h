#pragma once
#include "ITestUnit.h"

class AppendSpecialPagesTest : public ITestUnit
{
public:
	AppendSpecialPagesTest(void);
	virtual ~AppendSpecialPagesTest(void);

	virtual EStatusCode Run();
};
