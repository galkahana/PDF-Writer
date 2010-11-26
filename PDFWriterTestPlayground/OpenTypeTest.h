#pragma once
#include "ITestUnit.h"

class CFFFileInput;

class OpenTypeTest : public ITestUnit
{
public:
	OpenTypeTest(void);
	~OpenTypeTest(void);

	virtual EStatusCode Run();

private:
	EStatusCode SaveCharstringCode(unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput);
	EStatusCode TestFont();
};
