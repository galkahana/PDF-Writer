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
#include "PDFInteger.h"
#include "PDFBoolean.h"


#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace PDFHummus;

UsedFontsRepository::UsedFontsRepository(void)
{
	mInputFontsInformation = NULL;
	mObjectsContext = NULL;
	mEmbedFonts = true;
}

UsedFontsRepository::~UsedFontsRepository(void)
{
	Reset();
}

void UsedFontsRepository::SetObjectsContext(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
}

void UsedFontsRepository::SetEmbedFonts(bool inEmbedFonts) {
	mEmbedFonts = inEmbedFonts;
}

PDFUsedFont* UsedFontsRepository::GetFontForFile(const std::string& inFontFilePath,const std::string& inOptionalMetricsFile,long inFontIndex)
{
	if(!mObjectsContext)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForFile, exception, not objects context available");
		return NULL;
	}

	StringAndLongToPDFUsedFontMap::iterator it = mUsedFonts.find(StringAndLong(inFontFilePath,inFontIndex));
	if(it != mUsedFonts.end())
		return it->second;

	if(!mInputFontsInformation)
		mInputFontsInformation = new FreeTypeWrapper();


	FT_Face face;
	if(inOptionalMetricsFile.size() > 0)
	{
		face = mInputFontsInformation->NewFace(inFontFilePath,inOptionalMetricsFile,inFontIndex);
		mOptionaMetricsFiles.insert(StringToStringMap::value_type(inFontFilePath,inOptionalMetricsFile));
	} else {
		face = mInputFontsInformation->NewFace(inFontFilePath,inFontIndex);
	}
	if(!face) {
		TRACE_LOG1("UsedFontsRepository::GetFontForFile, Failed to load font from %s",inFontFilePath.c_str());
		return NULL;
	}

	PDFUsedFont* usedFont = new PDFUsedFont(face, mInputFontsInformation,inOptionalMetricsFile,inFontIndex,mObjectsContext,mEmbedFonts);
	if(!usedFont->IsValid())
	{
		TRACE_LOG1("UsedFontsRepository::GetFontForFile, Unrecognized font format for font in %s",inFontFilePath.c_str());
		delete usedFont;
		usedFont = NULL;
	}
	it = mUsedFonts.insert(StringAndLongToPDFUsedFontMap::value_type(StringAndLong(inFontFilePath,inFontIndex),usedFont)).first;
	return it->second;
}

EStatusCode UsedFontsRepository::WriteUsedFontsDefinitions()
{
	StringAndLongToPDFUsedFontMap::iterator it = mUsedFonts.begin();
	EStatusCode status = PDFHummus::eSuccess;

	for(; it != mUsedFonts.end() && PDFHummus::eSuccess == status; ++it)
		status = it->second ?
                    it->second->WriteFontDefinition():
                    eFailure;

	return status;
}

PDFUsedFont* UsedFontsRepository::GetFontForFile(const std::string& inFontFilePath,long inFontIndex)
{
	return GetFontForFile(inFontFilePath,"",inFontIndex);
}

static std::string scMemoryFontPrefix = "memory-font-";

PDFUsedFont* UsedFontsRepository::GetFontForFile(const std::vector<IOBasicTypes::Byte>& inFontBuffer,long inFontIndex)
{
	if(!mObjectsContext)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForFile, exception, not objects context available");
		return NULL;
	}

	if(!mInputFontsInformation)
		mInputFontsInformation = new FreeTypeWrapper();

	// load face first to derive a stable cache key from the PostScript name
	FT_Face face = mInputFontsInformation->NewFace(inFontBuffer,inFontIndex);
	if(!face)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForFile, Failed to load memory font");
		return NULL;
	}

	const char* psName = FT_Get_Postscript_Name(face);
	std::string key = scMemoryFontPrefix + (psName ? psName : "unknown") + "-" + std::to_string(inFontBuffer.size());

	StringAndLongToPDFUsedFontMap::iterator it = mUsedFonts.find(StringAndLong(key,inFontIndex));
	if(it != mUsedFonts.end())
	{
		mInputFontsInformation->DoneFace(face); // already got an existing font for this key, cleanup this new instances streams
		return it->second;
	}

	PDFUsedFont* usedFont = new PDFUsedFont(face,mInputFontsInformation,"",inFontIndex,mObjectsContext,mEmbedFonts);
	if(!usedFont->IsValid())
	{
		TRACE_LOG1("UsedFontsRepository::GetFontForFile, Unrecognized memory font format for key %s",key.c_str());
		delete usedFont;
		usedFont = NULL;
	}
	
	it = mUsedFonts.insert(StringAndLongToPDFUsedFontMap::value_type(StringAndLong(key,inFontIndex),usedFont)).first;

	return it->second;
}

PDFUsedFont* UsedFontsRepository::GetFontForStream(IByteReaderWithPosition* inFontStream,long inFontIndex) {
	if(!mObjectsContext)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForStream, exception, not objects context available");
		return NULL;
	}

	if(!mInputFontsInformation)
		mInputFontsInformation = new FreeTypeWrapper();

	FT_Face face = mInputFontsInformation->NewFace(inFontStream,inFontIndex);
	if(!face)
	{
		TRACE_LOG("UsedFontsRepository::GetFontForStream, Failed to load memory font");
		return NULL;
	}

	const char* psName = FT_Get_Postscript_Name(face);
	std::string key = scMemoryFontPrefix + (psName ? psName : "unknown");

	StringAndLongToPDFUsedFontMap::iterator it = mUsedFonts.find(StringAndLong(key,inFontIndex));
	if(it != mUsedFonts.end())
	{
		mInputFontsInformation->DoneFace(face); // already got an existing font for this key, cleanup this new instances streams
		return it->second;
	}

	PDFUsedFont* usedFont = new PDFUsedFont(face,mInputFontsInformation,"",inFontIndex,mObjectsContext,mEmbedFonts);
	if(!usedFont->IsValid())
	{
		TRACE_LOG1("UsedFontsRepository::GetFontForFile, Unrecognized memory font format for key %s",key.c_str());
		delete usedFont;
		usedFont = NULL;
	}
	
	it = mUsedFonts.insert(StringAndLongToPDFUsedFontMap::value_type(StringAndLong(key,inFontIndex),usedFont)).first;

	return it->second;
}

typedef std::list<ObjectIDType> ObjectIDTypeList;
EStatusCode UsedFontsRepository::WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	EStatusCode status = PDFHummus::eSuccess;
	ObjectIDTypeList usedFontsObjects;

	status = inStateWriter->StartNewIndirectObject(inObjectID);
	if(status != eSuccess)
		return status;
	DictionaryContext* usedFontsRepositoryObject = inStateWriter->StartDictionary();

	usedFontsRepositoryObject->WriteKey("Type");
	usedFontsRepositoryObject->WriteNameValue("UsedFontsRepository");

	usedFontsRepositoryObject->WriteKey("mEmbedFonts");
	usedFontsRepositoryObject->WriteBooleanValue(mEmbedFonts);

	usedFontsRepositoryObject->WriteKey("mUsedFonts");
	inStateWriter->StartArray();


	StringAndLongToPDFUsedFontMap::iterator it = mUsedFonts.begin();

	for(; it != mUsedFonts.end();++it)
	{
		std::string key = it->first.first;
		if(key.substr(scMemoryFontPrefix.size()) == scMemoryFontPrefix) {
			TRACE_LOG("UsedFontsRepository::WriteState, Memory font detected, not supported for writing state. Only file path fonts are supported for saving and restoring state");
			status = PDFHummus::eFailure;
			break;
		}
		
		PDFTextString aTextString(it->first.first);
		inStateWriter->WriteLiteralString(aTextString.ToString());
        
        inStateWriter->WriteInteger(it->first.second);
		
		ObjectIDType usedFontID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
		inStateWriter->WriteNewIndirectObjectReference(usedFontID);
		usedFontsObjects.push_back(usedFontID);
	}
	if(status != eSuccess)
		return status;

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
	StringAndLongToPDFUsedFontMap::iterator itUsedFonts = mUsedFonts.begin();
	for(; itUsedFonts != mUsedFonts.end();++itUsedFonts)
		delete (itUsedFonts->second);
	mUsedFonts.clear(); 


	PDFObjectCastPtr<PDFDictionary> usedFontsRepositoryState(inStateReader->ParseNewObject(inObjectID));

	PDFObjectCastPtr<PDFBoolean> embedFontsObject(usedFontsRepositoryState->QueryDirectObject("mEmbedFonts"));
	mEmbedFonts = embedFontsObject->GetValue();

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
	PDFObjectCastPtr<PDFLiteralString> keyStringItem;
    PDFObjectCastPtr<PDFInteger> keyIndexItem;
	PDFObjectCastPtr<PDFIndirectObjectReference> valueItem;

	if(!mInputFontsInformation)
		mInputFontsInformation = new FreeTypeWrapper();

	while(it.MoveNext() && PDFHummus::eSuccess == status)
	{
		keyStringItem = it.GetItem();
		it.MoveNext();
        keyIndexItem = it.GetItem();
        it.MoveNext();
		valueItem = it.GetItem();

		PDFTextString aTextString(keyStringItem->GetValue());
		std::string filePath = aTextString.ToUTF8String();
		if(filePath.substr(scMemoryFontPrefix.size()) == scMemoryFontPrefix) {
			TRACE_LOG("UsedFontsRepository::ReadState, Memory font detected, not supported for restoring state, used file path fonts only");
			status = PDFHummus::eFailure;
			break;
		}
			


        long fontIndex = (long)keyIndexItem->GetValue();

		FT_Face face;
		face = mInputFontsInformation->NewFace(filePath,fontIndex);
		
		if(!face)
		{
			TRACE_LOG2("UsedFontsRepository::ReadState, Failed to load font from %s at index %ld",filePath.c_str(),fontIndex);
			status = PDFHummus::eFailure;
			break;
		}


		PDFUsedFont* usedFont;
		
		StringToStringMap::iterator itOptionlMetricsFile = mOptionaMetricsFiles.find(filePath);
		if(itOptionlMetricsFile != mOptionaMetricsFiles.end())
			usedFont = new PDFUsedFont(face,mInputFontsInformation,itOptionlMetricsFile->second,fontIndex,mObjectsContext,mEmbedFonts);
		else
			usedFont = new PDFUsedFont(face,mInputFontsInformation,"",fontIndex,mObjectsContext, mEmbedFonts);
		if(!usedFont->IsValid())
		{
			TRACE_LOG2("UsedFontsRepository::ReadState, Unreckognized font format for font in %s at index %ld",filePath.c_str(),fontIndex);
			delete usedFont;
			usedFont = NULL;
			status = PDFHummus::eFailure;
			break;
		}

		usedFont->ReadState(inStateReader,valueItem->mObjectID);
		mUsedFonts.insert(StringAndLongToPDFUsedFontMap::value_type(StringAndLong(filePath,fontIndex),usedFont));

	}
	return status;
	
}

void UsedFontsRepository::Reset()
{
	StringAndLongToPDFUsedFontMap::iterator it = mUsedFonts.begin();
	for(; it != mUsedFonts.end();++it)
		delete (it->second);
	mUsedFonts.clear(); 
	delete mInputFontsInformation;
	mInputFontsInformation = NULL;
	mOptionaMetricsFiles.clear();
	mEmbedFonts = true;
}