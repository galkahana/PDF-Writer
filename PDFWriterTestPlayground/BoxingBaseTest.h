#pragma once
#include "ITestUnit.h"

class BoxingBaseTest : public ITestUnit
{
public:
	BoxingBaseTest(void);
	~BoxingBaseTest(void);

	virtual EStatusCode Run();

private:
	EStatusCode RunIntTest();
};
