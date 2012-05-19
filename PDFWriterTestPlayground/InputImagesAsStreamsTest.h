#pragma once
#include "ITestUnit.h"

class InputImagesAsStreamsTest : public ITestUnit
{
public:
	InputImagesAsStreamsTest(void);
	~InputImagesAsStreamsTest(void);

	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);
};
