#pragma once
#include "ITestUnit.h"

class CustomLogTest : public ITestUnit
{
public:
	CustomLogTest(void);
	~CustomLogTest(void);

	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);
};
