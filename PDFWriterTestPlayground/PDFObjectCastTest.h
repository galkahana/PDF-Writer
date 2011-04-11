#pragma once

#include "TestsRunner.h"

class PDFObjectCastTest : public ITestUnit
{
public:
	PDFObjectCastTest(void);
	virtual ~PDFObjectCastTest(void);

	virtual EStatusCode Run();
};
