#pragma once
#include "ITestUnit.h"

class PDFEmbedTest : public ITestUnit
{
public:
	PDFEmbedTest(void);
	virtual ~PDFEmbedTest(void);

	virtual EStatusCode Run();
};
