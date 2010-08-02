#pragma once
#include "ITestUnit.h"

class PDFWriter;

class TIFFImageTest : public ITestUnit
{
public:
	TIFFImageTest(void);
	virtual ~TIFFImageTest(void);

	virtual EStatusCode Run();

private:
	EStatusCode AddPageForTIFF(PDFWriter& inPDFWriter, const wchar_t* inTiffFilePath);
};
