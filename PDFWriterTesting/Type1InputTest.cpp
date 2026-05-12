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

   Also covers V-086: AddDependentGlyphs / CollectComponentGlyphs used to
   recurse on the Type 1 seac dependency graph without a visited-set
   guard or a depth cap, so a self-referencing or cyclic seac chain drove
   the call stack until it overflowed. Synthetic PFBs are produced in-
   process by Type1SyntheticBuilder.
*/
#include "InputFile.h"
#include "Type1Input.h"
#include "Type1SyntheticBuilder.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

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

// V-086 helpers
//
// Type1SyntheticBuilder::WithCharStrings produces a PFB where the glyph
// named in each entry maps verbatim to /CharStrings. Type 1 seac (operator
// 12 6) takes 5 operands (asb adx ady bchar achar); the parser only uses
// the last two as standard-encoding indices. We synthesise self-referencing
// or cyclic seac payloads by choosing bchar/achar codes that resolve back
// through StandardEncoding to the same glyph names embedded in the PFB.

// Plaintext Type 1 charstring bytes for a 4-arg seac with bchar = achar =
// inCode. Pushes (0, 0, 0, code, code) then opcode escape+6 (= seac). Codes
// in [-107, 107] encode as a single byte = code + 139.
static string MakeSelfSeacPlaintext(Byte inCode) {
	const char one = (char)((int)inCode + 139);
	char bytes[] = { '\x8B', '\x8B', '\x8B', one, one, '\x0C', '\x06' };
	return string(bytes, sizeof(bytes));
}

static bool ContainsString(const vector<string>& inGlyphs, const string& inName) {
	for(size_t i = 0; i < inGlyphs.size(); ++i)
		if(inGlyphs[i] == inName) return true;
	return false;
}

// V-086: glyph "space" seac-refs standard encoding code 32 ("space"), so the
// dependency walk recurses on the same glyph. Pre-fix this overflowed the
// call stack on the single-glyph self-reference.
static bool AddDependentGlyphs_SelfReferencingSeac_TerminatesWithSelfDependency() {
	// Arrange
	vector<Type1SyntheticBuilder::NamedCharString> glyphs;
	glyphs.push_back(make_pair(string("space"), MakeSelfSeacPlaintext(32)));
	string pfb = Type1SyntheticBuilder::WithCharStrings(glyphs);

	Type1Input type1;
	if(Type1SyntheticBuilder::ParseAsType1(pfb, type1) != eSuccess) {
		cout << "Type1InputTest [AddDependentGlyphs::SelfReferencingSeac_TerminatesWithSelfDependency]: synthetic PFB parse failed" << endl;
		return false;
	}

	// Act
	vector<string> subset;
	subset.push_back("space");
	EStatusCode status = type1.AddDependentGlyphs(subset);

	// Assert
	if(status != eSuccess) {
		cout << "Type1InputTest [AddDependentGlyphs::SelfReferencingSeac_TerminatesWithSelfDependency]: status not eSuccess" << endl;
		return false;
	}
	if(subset.size() != 1 || subset[0] != "space") {
		cout << "Type1InputTest [AddDependentGlyphs::SelfReferencingSeac_TerminatesWithSelfDependency]: subset != {space}" << endl;
		return false;
	}
	return true;
}

// V-086: two glyphs whose seac dependencies form a cycle ("space" -> "exclam",
// "exclam" -> "space"). Pre-fix the recursion ping-ponged between them
// until the stack overflowed.
static bool AddDependentGlyphs_TwoGlyphCycle_TerminatesWithBothDependencies() {
	// Arrange: "space" seacs code 33 ("exclam"); "exclam" seacs code 32
	// ("space").
	vector<Type1SyntheticBuilder::NamedCharString> glyphs;
	glyphs.push_back(make_pair(string("space"), MakeSelfSeacPlaintext(33)));
	glyphs.push_back(make_pair(string("exclam"), MakeSelfSeacPlaintext(32)));
	string pfb = Type1SyntheticBuilder::WithCharStrings(glyphs);

	Type1Input type1;
	if(Type1SyntheticBuilder::ParseAsType1(pfb, type1) != eSuccess) {
		cout << "Type1InputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: synthetic PFB parse failed" << endl;
		return false;
	}

	// Act
	vector<string> subset;
	subset.push_back("space");
	EStatusCode status = type1.AddDependentGlyphs(subset);

	// Assert
	if(status != eSuccess) {
		cout << "Type1InputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: status not eSuccess" << endl;
		return false;
	}
	if(subset.size() != 2) {
		cout << "Type1InputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: subset size " << subset.size() << ", expected 2" << endl;
		return false;
	}
	if(!ContainsString(subset, "space") || !ContainsString(subset, "exclam")) {
		cout << "Type1InputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: subset missing one of {space, exclam}" << endl;
		return false;
	}
	return true;
}

// V-086: a 25-deep acyclic seac chain that would push the call stack past
// reasonable limits without a depth cap. The cap stops the walk before the
// tail glyph is reached. The chain is built over standard-encoding glyph
// names: "space" -> "exclam" -> "quotedbl" -> ... -> chainLength glyphs.
// We assert the tail wasn't reached without baking the exact cap value
// into the assertion so future tuning of scMaxCompositeDepth doesn't
// break this test.
static const char* const scStdEncodingNames32To56[] = {
	"space", "exclam", "quotedbl", "numbersign", "dollar", "percent", "ampersand",
	"quoteright", "parenleft", "parenright", "asterisk", "plus", "comma", "hyphen",
	"period", "slash", "zero", "one", "two", "three", "four", "five", "six", "seven",
	"eight"
};

static bool AddDependentGlyphs_DeepAcyclicChain_StopsBeforeChainEnd() {
	// Arrange
	const size_t chainLength = sizeof(scStdEncodingNames32To56) / sizeof(scStdEncodingNames32To56[0]);
	vector<Type1SyntheticBuilder::NamedCharString> glyphs;
	for(size_t i = 0; i + 1 < chainLength; ++i) {
		// glyph at index i (standard encoding 32+i) chains to index i+1.
		glyphs.push_back(make_pair(string(scStdEncodingNames32To56[i]),
		                           MakeSelfSeacPlaintext((Byte)(32 + i + 1))));
	}
	// Tail glyph: plain endchar (no seac), terminates the chain in the
	// no-cap world. With the cap, the walk stops before reaching it.
	glyphs.push_back(make_pair(string(scStdEncodingNames32To56[chainLength - 1]),
	                           string("\x0E", 1)));

	string pfb = Type1SyntheticBuilder::WithCharStrings(glyphs);
	Type1Input type1;
	if(Type1SyntheticBuilder::ParseAsType1(pfb, type1) != eSuccess) {
		cout << "Type1InputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: synthetic PFB parse failed" << endl;
		return false;
	}

	// Act
	vector<string> subset;
	subset.push_back(scStdEncodingNames32To56[0]);
	EStatusCode status = type1.AddDependentGlyphs(subset);

	// Assert
	if(status != eSuccess) {
		cout << "Type1InputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: status not eSuccess" << endl;
		return false;
	}
	if(!ContainsString(subset, scStdEncodingNames32To56[0])) {
		cout << "Type1InputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: input glyph missing from subset" << endl;
		return false;
	}
	if(ContainsString(subset, scStdEncodingNames32To56[chainLength - 1])) {
		cout << "Type1InputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: tail glyph was reached; depth cap did not fire" << endl;
		return false;
	}
	return true;
}

static bool RunAddDependentGlyphsCases() {
	if(!AddDependentGlyphs_SelfReferencingSeac_TerminatesWithSelfDependency()) return false;
	if(!AddDependentGlyphs_TwoGlyphCycle_TerminatesWithBothDependencies()) return false;
	if(!AddDependentGlyphs_DeepAcyclicChain_StopsBeforeChainEnd()) return false;
	return true;
}

int Type1InputTest(int argc, char* argv[]) {
	(void) argc;
	if(!ReadType1File_RealPFB_ParsesFontInfoStrings(argv)) return 1;
	if(!RunAddDependentGlyphsCases()) return 1;
	return 0;
}
