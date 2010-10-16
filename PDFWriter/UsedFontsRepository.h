#pragma once

#include "EStatusCode.h"

#include <string>
#include <map>

using namespace std;

class FreeTypeWrapper;
class PDFUsedFont;
class ObjectsContext;

typedef map<wstring,PDFUsedFont*> WStringToPDFUsedFontMap;

class UsedFontsRepository
{
public:
	UsedFontsRepository(void);
	~UsedFontsRepository(void);

	void SetObjectsContext(ObjectsContext* inObjectsContext);

	PDFUsedFont* GetFontForFile(const wstring& inFontFilePath);
	// second overload is for type 1, when an additional metrics file is available
	PDFUsedFont* GetFontForFile(const wstring& inFontFilePath,const wstring& inOptionalMetricsFile);

	EStatusCode WriteUsedFontsDefinitions();

private:

	ObjectsContext* mObjectsContext;
	FreeTypeWrapper* mInputFontsInformation;
	WStringToPDFUsedFontMap mUsedFonts;
};
