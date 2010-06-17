#pragma once
#include "ITestUnit.h"

class JPGImageTest : public ITestUnit
{
public:
	JPGImageTest(void);
	virtual ~JPGImageTest(void);

	virtual EStatusCode Run();
};
