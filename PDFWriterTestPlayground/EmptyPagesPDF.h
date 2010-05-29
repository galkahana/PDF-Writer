#pragma once
#include "ITestUnit.h"

class EmptyPagesPDF : public ITestUnit
{
public:
	EmptyPagesPDF(void);
	~EmptyPagesPDF(void);

	virtual EStatusCode Run();

};
