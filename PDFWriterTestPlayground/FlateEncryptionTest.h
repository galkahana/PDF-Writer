#pragma once

#include "TestsRunner.h"

class FlateEncryptionTest : public ITestUnit
{
public:
	FlateEncryptionTest(void);
	virtual ~FlateEncryptionTest(void);

	virtual EStatusCode Run();
};
