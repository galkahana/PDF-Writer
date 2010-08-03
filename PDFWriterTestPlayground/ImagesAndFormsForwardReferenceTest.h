#pragma once

#include "ITestUnit.h"

class ImagesAndFormsForwardReferenceTest: public ITestUnit
{
public:
	ImagesAndFormsForwardReferenceTest(void);
	virtual ~ImagesAndFormsForwardReferenceTest(void);

	virtual EStatusCode Run();
};
