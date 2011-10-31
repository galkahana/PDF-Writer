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
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFTextString.h"
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFArray.h"
#include "PDFIndirectObjectReference.h"
#include "PDFLiteralString.h"


#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace PDFHummus;

UsedFontsRepository::UsedFontsRepository(void)
{
	mInputFontsInformation = NULL;
	mObjectsContext = NULL;
}

UsedFontsRepository::~UsedFontsRepository(void)
{
	StringToPDFUsedFontMap::iterator it = mUsedFonts.begin();
	for(; it != mUsedFonts.end();++it)
		delete (it->second);
	mUsedFonts.clear();
	delete mInputFontsInformation;
}

void UsedFontsRepository::SetObjectsContext(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
}

PDFUsedFont* UsedFontsRepository::GetFontForFile(const string& inFontFilePath,const string& inOptionalMetricsFile)
{
	if(!mObjectsContext)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForFile, exception, not objects context available");
		return NULL;
	}

	StringToPDFUsedFontMap::iterator it = mUsedFonts.find(inFontFilePath);
	if(it == mUsedFonts.end())
	{
		if(!mInputFontsInformation)
			mInputFontsInformation = new FreeTypeWrapper();


		FT_Face face;
		if(inOptionalMetricsFile.size() > 0)
		{
			face = mInputFontsInformation->NewFace(inFontFilePath,inOptionalMetricsFile);
			mOptionaMetricsFiles.insert(StringToStringMap::value_type(inFontFilePath,inOptionalMetricsFile));
		}
		else
			face = mInputFontsInformation->NewFace(inFontFilePath);
		if(!face)
		{
			TRACE_LOG1("UsedFontsRepository::GetFontForFile, Failed to load font from %s",inFontFilePath.c_str());
			PDFUsedFont* aNull = NULL;
			it = mUsedFonts.insert(StringToPDFUsedFontMap::value_type(inFontFilePath,aNull)).first;
		}
		else
		{

			PDFUsedFont* usedFont = new PDFUsedFont(face,inFontFilePath,inOptionalMetricsFile,mObjectsContext);
			if(!usedFont->IsValid())
			{
				TRACE_LOG1("UsedFontsRepository::GetFontForFile, Unreckognized font format for font in %s",inFontFilePath.c_str());
				delete usedFont;
				usedFont = NULL;
			}
			it = mUsedFonts.insert(StringToPDFUsedFontMap::value_type(inFontFilePath,usedFont)).first;

		}
	}
	return it->second;
}

EStatusCode UsedFontsRepository::WriteUsedFontsDefinitions()
{
	StringToPDFUsedFontMap::iterator it = mUsedFonts.begin();
	EStatusCode status = PDFHummus::eSuccess;

	for(; it != mUsedFonts.end() && PDFHummus::eSuccess == status; ++it)
		status = it->second->WriteFontDefinition();

	return status;
}

PDFUsedFont* UsedFontsRepository::GetFontForFile(const string& inFontFilePath)
{
	return GetFontForFile(inFontFilePath,"");
}

typedef list<ObjectIDType> ObjectIDTypeList;
EStatusCode UsedFontsRepository::WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	EStatusCode status = PDFHummus::eSuccess;
	ObjectIDTypeList usedFontsObjects;

	inStateWriter->StartNewIndirectObject(inObjectID);
	DictionaryContext* usedFontsRepositoryObject = inStateWriter->StartDictionary();

	usedFontsRepositoryObject->WriteKey("Type");
	usedFontsRepositoryObject->WriteNameValue("UsedFontsRepository");

	usedFontsRepositoryObject->WriteKey("mUsedFonts");
	inStateWriter->StartArray();


	StringToPDFUsedFontMap::iterator it = mUsedFonts.begin();

	for(; it != mUsedFonts.end();++it)
	{
		PDFTextString aTextString(it->first);
		inStateWriter->WriteLiteralString(aTextString.ToString());

		ObjectIDType usedFontID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
		inStateWriter->WriteIndirectObjectReference(usedFontID);
		usedFontsObjects.push_back(usedFontID);
	}

	inStateWriter->EndArray(eTokenSeparatorEndLine);

	usedFontsRepositoryObject->WriteKey("mOptionaMetricsFiles");
	inStateWriter->StartArray();

	StringToStringMap::iterator itOptionals = mOptionaMetricsFiles.begin();
	for(; itOptionals != mOptionaMetricsFiles.end();++itOptionals)
	{
		PDFTextString aTextString(itOptionals->first);
		inStateWriter->WriteLiteralString(aTextString.ToString());

		aTextString = itOptionals->second;
		inStateWriter->WriteLiteralString(aTextString.ToString());
	}

	inStateWriter->EndArray(eTokenSeparatorEndLine);

	inStateWriter->EndDictionary(usedFontsRepositoryObject);
	inStateWriter->EndIndirectObject();

	if(usedFontsObjects.size() > 0)
	{
		it = mUsedFonts.begin();
		ObjectIDTypeList::iterator itIDs = usedFontsObjects.begin();

		for(; it != mUsedFonts.end() && PDFHummus::eSuccess == status;++it,++itIDs)
			status = it->second->WriteState(inStateWriter,*itIDs);
	}

	return status;
}

EStatusCode UsedFontsRepository::ReadState(PDFParser* inStateReader,ObjectIDType inObjectID)
{
	EStatusCode status = PDFHummus::eSuccess;

	// clear current state
	StringToPDFUsedFontMap::iterator itUsedFonts = mUsedFonts.begin();
	for(; itUsedFonts != mUsedFonts.end();++itUsedFonts)
		delete (itUsedFonts->second);
	mUsedFonts.clear();


	PDFObjectCastPtr<PDFDictionary> usedFontsRepositoryState(inStateReader->ParseNewObject(inObjectID));

	mOptionaMetricsFiles.clear();
	PDFObjectCastPtr<PDFArray> optionalMetricsState(usedFontsRepositoryState->QueryDirectObject("mOptionaMetricsFiles"));
	SingleValueContainerIterator<PDFObjectVector> it = optionalMetricsState->GetIterator();
	PDFObjectCastPtr<PDFLiteralString> aStringValue;

	while(it.MoveNext())
	{
		PDFTextString aKey;

		aStringValue = it.GetItem();
		aKey = aStringValue->GetValue();

		PDFTextString aValue;

		it.MoveNext();
		aStringValue = it.GetItem();
		aValue = aStringValue->GetValue();

		mOptionaMetricsFiles.insert(StringToStringMap::value_type(aKey.ToUTF8String(),aValue.ToUTF8String()));
	}

	PDFObjectCastPtr<PDFArray> usedFontsState(usedFontsRepositoryState->QueryDirectObject("mUsedFonts"));

	it = usedFontsState->GetIterator();
	PDFObjectCastPtr<PDFLiteralString> keyItem;
	PDFObjectCastPtr<PDFIndirectObjectReference> valueItem;

	if(!mInputFontsInformation)
		mInputFontsInformation = new FreeTypeWrapper();

	while(it.MoveNext() && PDFHummus::eSuccess == status)
	{
		keyItem = it.GetItem();
		it.MoveNext();
		valueItem = it.GetItem();

		PDFTextString aTextString(keyItem->GetValue());
		string filePath = aTextString.ToUTF8String();

		FT_Face face;
		face = mInputFontsInformation->NewFace(filePath);

		if(!face)
		{
			TRACE_LOG1("UsedFontsRepository::ReadState, Failed to load font from %s",filePath.c_str());
			status = PDFHummus::eFailure;
			break;
		}


		PDFUsedFont* usedFont;

		StringToStringMap::iterator itOptionlMetricsFile = mOptionaMetricsFiles.find(filePath);
		if(itOptionlMetricsFile != mOptionaMetricsFiles.end())
			usedFont = new PDFUsedFont(face,filePath,itOptionlMetricsFile->second,mObjectsContext);
		else
			usedFont = new PDFUsedFont(face,filePath,"",mObjectsContext);
		if(!usedFont->IsValid())
		{
			TRACE_LOG1("UsedFontsRepository::ReadState, Unreckognized font format for font in %s",filePath.c_str());
			delete usedFont;
			usedFont = NULL;
			status = PDFHummus::eFailure;
			break;
		}

		usedFont->ReadState(inStateReader,valueItem->mObjectID);
		mUsedFonts.insert(StringToPDFUsedFontMap::value_type(filePath,usedFont));

	}
	return status;

}
