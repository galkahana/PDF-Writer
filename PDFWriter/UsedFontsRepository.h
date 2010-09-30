#pragma once

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


private:

	ObjectsContext* mObjectsContext;
	FreeTypeWrapper* mInputFontsInformation;
	WStringToPDFUsedFontMap mUsedFonts;
};
