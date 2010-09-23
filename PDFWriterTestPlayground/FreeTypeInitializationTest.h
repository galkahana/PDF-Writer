#pragma once
#include "TestsRunner.h"

#include "FreeTypeWrapper.h"



class FreeTypeInitializationTest: public ITestUnit
{
public:
	FreeTypeInitializationTest(void);
	~FreeTypeInitializationTest(void);

	virtual EStatusCode Run();


private:
	EStatusCode ShowFaceProperties(FreeTypeWrapper& inFreeType,const wchar_t* inFontFilePath,const wchar_t* inSecondaryFontFilePath = NULL);
	EStatusCode ShowGlobalFontProperties(FreeTypeWrapper& inFreeType,FT_Face inFace);
};
