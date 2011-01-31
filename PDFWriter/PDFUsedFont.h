/*
   Source File : PDFUsedFont.h


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

#include "FreeTypeFaceWrapper.h"
#include "ObjectsBasicTypes.h"
#include "EStatusCode.h"
#include "GlyphUnicodeMapping.h"
#include <string>
#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

typedef list<unsigned short> UShortList;
typedef list<UShortList> UShortListList;
typedef list<wstring> WStringList;
typedef list<GlyphUnicodeMappingList> GlyphUnicodeMappingListList;

class IWrittenFont;
class ObjectsContext;

class PDFUsedFont
{
public:
	PDFUsedFont(FT_Face inInputFace,
				const wstring& inFontFilePath,
				const wstring& inAdditionalMetricsFontFilePath,
				ObjectsContext* inObjectsContext);
	virtual ~PDFUsedFont(void);

	bool IsValid();

	// a String in the following two implementations is represented here by a list of glyphs, with each mapped to the matching
	// unicode values. to move from a wide string to such a structure, use other class methods

	/*
		This function does the work of encoding a text string to a matching font instance name, and an encoded array
		of characters. the encoded array is a list of short values (double byte for CID, one byte for regular), and an extra
		boolean value indicates whether they are CID or regular.
	*/
	EStatusCode EncodeStringForShowing(const GlyphUnicodeMappingList& inText,
										ObjectIDType &outFontObjectToUse,
										UShortList& outCharactersToUse,
										bool& outTreatCharactersAsCID);

	// encode all strings. make sure that they will use the same font.
	EStatusCode EncodeStringsForShowing(const GlyphUnicodeMappingListList& inText,
										ObjectIDType &outFontObjectToUse,
										UShortListList& outCharactersToUse,
										bool& outTreatCharactersAsCID);

	EStatusCode WriteFontDefinition();

	// use this method to translate text to glyphs and unicode mapping, to be later used for EncodeStringForShowing
	EStatusCode TranslateStringToGlyphs(const wstring& inText,GlyphUnicodeMappingList& outGlyphsUnicodeMapping);

private:
	FreeTypeFaceWrapper mFaceWrapper;
	IWrittenFont* mWrittenFont;
	ObjectsContext* mObjectsContext;


};
