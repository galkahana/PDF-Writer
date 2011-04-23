/*
   Source File : UsedFontsRepository.h


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
#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"

#include <string>
#include <map>

using namespace std;

class FreeTypeWrapper;
class PDFUsedFont;
class ObjectsContext;
class PDFParser;

typedef map<wstring,PDFUsedFont*> WStringToPDFUsedFontMap;
typedef map<wstring,wstring> WStringToWStringMap;

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

	EStatusCode WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
	EStatusCode ReadState(PDFParser* inStateReader,ObjectIDType inObjectID);

private:

	ObjectsContext* mObjectsContext;
	FreeTypeWrapper* mInputFontsInformation;
	WStringToPDFUsedFontMap mUsedFonts;
	WStringToWStringMap mOptionaMetricsFiles;
};
