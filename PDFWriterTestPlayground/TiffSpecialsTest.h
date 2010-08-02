#pragma once
#include "ITestUnit.h"

class PDFWriter;
class PDFFormXObject;

class TiffSpecialsTest : public ITestUnit
{
public:
	TiffSpecialsTest(void);
	virtual ~TiffSpecialsTest(void);

	virtual EStatusCode Run();

private:
	EStatusCode CreatePageForImageAndRelease(PDFWriter& inPDFWriter,PDFFormXObject* inImageObject);
	EStatusCode CreateBlackAndWhiteMaskImage(PDFWriter& inPDFWriter);
	EStatusCode CreateBiLevelGrayScales(PDFWriter& inPDFWriter);
};