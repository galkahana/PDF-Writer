#pragma once
#include "ITestUnit.h"

class InputFlateDecodeTester : public ITestUnit
{
public:
	InputFlateDecodeTester(void);
	virtual ~InputFlateDecodeTester(void);

	virtual EStatusCode Run();
};
