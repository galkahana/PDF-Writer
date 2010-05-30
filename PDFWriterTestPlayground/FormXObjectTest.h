#pragma once
#include "ITestUnit.h"

class FormXObjectTest : public ITestUnit
{
public:
	FormXObjectTest(void);
	virtual ~FormXObjectTest(void);

	virtual EStatusCode Run();
};
