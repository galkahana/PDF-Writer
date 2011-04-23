#pragma once

#include "ITestUnit.h"

class PDFCopyingContextTest : public ITestUnit
{
public:
	PDFCopyingContextTest(void);
	virtual ~PDFCopyingContextTest(void);

	EStatusCode Run();
};
