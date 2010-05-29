#pragma once
#include "PDFDate.h"
#include "TestsRunner.h"


class PDFDateTest: public ITestUnit
{
public:
	PDFDateTest(void);
	~PDFDateTest(void);

	virtual EStatusCode Run();
};

