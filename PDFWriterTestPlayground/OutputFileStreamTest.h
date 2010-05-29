#pragma once

#include "TestsRunner.h"

class OutputFileStreamTest : public ITestUnit
{
public:
	OutputFileStreamTest(void);
	virtual ~OutputFileStreamTest(void);

	virtual EStatusCode Run();
};

