#pragma once
#include "ITestUnit.h"
#include "CFFFileInput.h"

class OpenTypeTest : public ITestUnit
{
public:
	OpenTypeTest(void);
	~OpenTypeTest(void);

	virtual EStatusCode Run();

private:
	EStatusCode SaveCharstringCode(unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput);
	EStatusCode TestFont();

	EStatusCode ExtractFontSegment();
	EStatusCode DisplayFontSegmentInformation();
	void DumpDictionary(const UShortToDictOperandListMap& inDict);
	wstring FormatDictionaryKey(unsigned short inDictionaryKey);
};
