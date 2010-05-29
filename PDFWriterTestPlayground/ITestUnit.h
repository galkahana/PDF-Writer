#pragma once

#include "EStatusCode.h"

class ITestUnit
{

public:
	
	virtual ~ITestUnit(){}

	virtual EStatusCode Run() = 0;

};