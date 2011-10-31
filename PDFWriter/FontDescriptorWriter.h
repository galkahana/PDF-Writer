/*
   Source File : FontDescriptorWriter.h


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

#include "ObjectsBasicTypes.h"
#include "WrittenFontRepresentation.h"

#include <string>
#include <vector>
#include <utility>

using namespace std;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class FreeTypeFaceWrapper;
class ObjectsContext;
class IFontDescriptorHelper;

class FontDescriptorWriter
{
public:
	FontDescriptorWriter(void);
	~FontDescriptorWriter(void);

	void WriteFontDescriptor(	ObjectIDType inFontDescriptorObjectID,
								const string& inFontPostscriptName,
								FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
								ObjectsContext* inObjectsContext,
								IFontDescriptorHelper* inDescriptorHelper);

private:
	unsigned int CalculateFlags(FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
	bool IsSymbolic(FreeTypeFaceWrapper* inFontInfo,
					const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
};
