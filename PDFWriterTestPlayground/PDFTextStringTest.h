#pragma once
#include "TestsRunner.h"

class PDFTextStringTest: public ITestUnit
{
public:
	PDFTextStringTest(void);
	~PDFTextStringTest(void);

	virtual EStatusCode Run();
};
