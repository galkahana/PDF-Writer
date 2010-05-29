#pragma once

#include "TestsRunner.h"

class BufferedOutputStreamTest: public ITestUnit
{
public:
	BufferedOutputStreamTest(void);
	virtual ~BufferedOutputStreamTest(void);

	virtual EStatusCode Run();
};


