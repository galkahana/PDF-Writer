#pragma once
#include "ITestUnit.h"

class TIFFImageTest : public ITestUnit
{
public:
	TIFFImageTest(void);
	virtual ~TIFFImageTest(void);

	virtual EStatusCode Run();
};
