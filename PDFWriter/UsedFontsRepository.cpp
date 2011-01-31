/*
   Source File : UsedFontsRepository.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
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
	WStringToPDFUsedFontMap::iterator it = mUsedFonts.begin();
	for(; it != mUsedFonts.end();++it)
		delete (it->second);
	mUsedFonts.clear(); 
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

			PDFUsedFont* usedFont = new PDFUsedFont(face,inFontFilePath,inOptionalMetricsFile,mObjectsContext);
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