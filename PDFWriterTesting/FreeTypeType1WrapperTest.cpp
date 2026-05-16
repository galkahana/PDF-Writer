/*
   Source File : FreeTypeType1WrapperTest.cpp


   Copyright 2026 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


   Regression tests for FreeTypeType1Wrapper's FreeType return-code handling:

     * V-082: FT_Get_PS_Font_Value's return value was ignored and
       T1_EncodingType was left uninitialized, so mIsCustomEncoding (which
       routes every glyph lookup) could be derived from a stack-garbage
       value.
     * V-083: InputFile::OpenFile's return value was ignored; ReadType1File
       ran against an unopened stream and glyph-name lookups then read out
       of an indeterminate Type1Input. The fix gates GetPrivateGlyphName on
       a load-succeeded flag.

   The pure failure branches (OpenFile failing while FreeType still loads
   the same face from the same path; FT_Get_PS_Font_Value failing on an
   otherwise valid Type 1 font) are not reproducible with a static fixture.
   This test therefore locks in that the defensive changes do not regress
   the valid-Type1 embedding path that the wrapper is built for.
*/
#include "FreeTypeFaceWrapper.h"
#include "FreeTypeWrapper.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// Loading HLB_____.PFB through FreeTypeFaceWrapper with its real font/PFM
// paths constructs a FreeTypeType1Wrapper that (a) reads PS_DICT_ENCODING_TYPE
// (V-082 init + return check) and (b) opens the PFB and parses it
// (V-083 OpenFile return check + mType1Loaded gate). A correct glyph name
// for 'A' proves the load path still works end to end.
static bool Construct_RealType1Font_ResolvesGlyphName(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/HLB_____.PFB"),
	                          BuildRelativeInputPath(argv, "fonts/HLB_____.PFM"),
	                          0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeType1WrapperTest [Construct::RealType1Font_ResolvesGlyphName]: failed to load HLB_____.PFB" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face,
		                            BuildRelativeInputPath(argv, "fonts/HLB_____.PFB"),
		                            BuildRelativeInputPath(argv, "fonts/HLB_____.PFM"),
		                            0, false);
		if(!wrapper.IsValid()) {
			cout << "FreeTypeType1WrapperTest [Construct::RealType1Font_ResolvesGlyphName]: wrapper not valid" << endl;
			break;
		}
		FT_UInt glyphIndex = FT_Get_Char_Index(face, 'A');
		if(glyphIndex == 0) {
			cout << "FreeTypeType1WrapperTest [Construct::RealType1Font_ResolvesGlyphName]: no glyph for 'A'" << endl;
			break;
		}

		// Act
		string name = wrapper.GetGlyphName(glyphIndex);

		// Assert
		if(name != "A") {
			cout << "FreeTypeType1WrapperTest [Construct::RealType1Font_ResolvesGlyphName]: expected \"A\", got \"" << name << "\"" << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

int FreeTypeType1WrapperTest(int argc, char* argv[]) {
	(void)argc;

	if(!Construct_RealType1Font_ResolvesGlyphName(argv)) return 1;
	return 0;
}
