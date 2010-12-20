#pragma once

#include "ITestUnit.h"

class PFBStreamTest : public ITestUnit
{
public:
	PFBStreamTest(void);
	~PFBStreamTest(void);

	virtual EStatusCode Run();
};
