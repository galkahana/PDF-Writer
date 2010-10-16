#include "UsedFontsRepository.h"
#include "FreeTypeWrapper.h"
#include "PDFUsedFont.h"
#include "Trace.h"

#include <ft2build.h>
#include FT_FREETYPE_H

UsedFontsRepository::UsedFontsRepository(void)
{
	mInputFontsInformation = NULL;
	mObjectsContext = NULL;
}

UsedFontsRepository::~UsedFontsRepository(void)
{
	delete mInputFontsInformation;
}

void UsedFontsRepository::SetObjectsContext(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
}

PDFUsedFont* UsedFontsRepository::GetFontForFile(const wstring& inFontFilePath,const wstring& inOptionalMetricsFile)
{
	if(!mObjectsContext)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForFile, exception, not objects context available");
		return NULL;
	}

	WStringToPDFUsedFontMap::iterator it = mUsedFonts.find(inFontFilePath);
	if(it == mUsedFonts.end())
	{
		if(!mInputFontsInformation)
			mInputFontsInformation = new FreeTypeWrapper();


		FT_Face face;
		if(inOptionalMetricsFile.size() > 0)
			face = mInputFontsInformation->NewFace(inFontFilePath,inOptionalMetricsFile);
		else
			face = mInputFontsInformation->NewFace(inFontFilePath);
		if(!face)
		{
			TRACE_LOG1("UsedFontsRepository::GetFontForFile, Failed to load font from %s",inFontFilePath);
			it = mUsedFonts.insert(WStringToPDFUsedFontMap::value_type(inFontFilePath,NULL)).first;
		}
		else
		{

			PDFUsedFont* usedFont = new PDFUsedFont(face,inOptionalMetricsFile,mObjectsContext);
			if(!usedFont->IsValid())
			{
				TRACE_LOG1("UsedFontsRepository::GetFontForFile, Unreckognized font format for font in %s",inFontFilePath);
				delete usedFont;
				usedFont = NULL;
			}
			it = mUsedFonts.insert(WStringToPDFUsedFontMap::value_type(inFontFilePath,usedFont)).first;

		}
	}
	return it->second;
}

EStatusCode UsedFontsRepository::WriteUsedFontsDefinitions()
{
	WStringToPDFUsedFontMap::iterator it = mUsedFonts.begin();
	EStatusCode status = eSuccess;

	for(; it != mUsedFonts.end() && eSuccess == status; ++it)
		status = it->second->WriteFontDefinition();

	return status;	
}

PDFUsedFont* UsedFontsRepository::GetFontForFile(const wstring& inFontFilePath)
{
	return GetFontForFile(inFontFilePath,L"");
}