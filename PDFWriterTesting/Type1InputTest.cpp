/*
   Source File : Type1InputTest.cpp


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


   End-to-end Type1Input regression tests. Driven through ReadType1File
   against the real Helvetica PFB to assert exact parsed values for the
   Font dictionary and FontInfo dictionary fields -- proves the boundary
   helpers in Type1PSTokens (IsComment / FromPSName / FromPSString) and
   the surrounding parser still consume legitimate input cleanly.
*/
#include "InputFile.h"
#include "Type1Input.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

static bool ReadType1File_RealPFB_ParsesFontInfoStrings(char* argv[]) {
	bool ok = false;
	InputFile pfb;
	Type1Input type1;
	do {
		// Arrange
		if(pfb.OpenFile(BuildRelativeInputPath(argv, "fonts/HLB_____.PFB")) != eSuccess) {
			cout << "Type1InputTest: failed to open HLB_____.PFB" << endl;
			break;
		}

		// Act
		if(type1.ReadType1File(pfb.GetInputStream()) != eSuccess) {
			cout << "Type1InputTest: ReadType1File rejected the real PFB" << endl;
			break;
		}

		// Assert (exact-value)
		if(type1.mFontDictionary.FontName != "HelveticaNeue-Bold") {
			cout << "Type1InputTest: FontName mismatch: \"" << type1.mFontDictionary.FontName << "\"" << endl;
			break;
		}
		if(type1.mFontInfoDictionary.version != "001.102") {
			cout << "Type1InputTest: version mismatch: \"" << type1.mFontInfoDictionary.version << "\"" << endl;
			break;
		}
		if(type1.mFontInfoDictionary.FullName != "Helvetica 75 Bold") {
			cout << "Type1InputTest: FullName mismatch: \"" << type1.mFontInfoDictionary.FullName << "\"" << endl;
			break;
		}
		if(type1.mFontInfoDictionary.FamilyName != "Helvetica Neue") {
			cout << "Type1InputTest: FamilyName mismatch: \"" << type1.mFontInfoDictionary.FamilyName << "\"" << endl;
			break;
		}
		if(type1.mFontInfoDictionary.Weight != "Bold") {
			cout << "Type1InputTest: Weight mismatch: \"" << type1.mFontInfoDictionary.Weight << "\"" << endl;
			break;
		}
		ok = true;
	} while(false);

	pfb.CloseFile();
	return ok;
}

int Type1InputTest(int argc, char* argv[]) {
	(void) argc;
	if(!ReadType1File_RealPFB_ParsesFontInfoStrings(argv)) return 1;
	return 0;
}
