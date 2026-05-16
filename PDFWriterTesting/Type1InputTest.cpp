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

// V-072: the dictionary parse loops fed GetNextToken().second straight into
// Int()/Double()/FromPS*() without checking GetNextToken().first. A key whose
// value token is absent (truncated font) was silently accepted -- the boxed
// converter yielded 0 / empty and parsing reported eSuccess. Now a missing
// value token flags failure and the parse returns non-eSuccess.

// Each case feeds a raw ASCII segment ending in a dangling key (no value
// token, segment then ends) and expects the parse to fail rather than
// silently accept. The two rows enter the fix through the two top-level
// dictionary entry points: "begin" -> ReadFontDictionary, "/Private" ->
// ReadPrivateDictionary.
struct MissingValueTokenCase {
	const char* label;       // <Condition>_<Result>
	const char* ascii;       // raw ASCII segment, dangling key last
	const char* danglingKey; // for the failure message
};

static const MissingValueTokenCase scMissingValueTokenCases[] = {
	{"FontDictionaryPath_Fails",    "12 dict begin\n/PaintType",        "/PaintType"},
	{"PrivateDictionaryPath_Fails", "/Private 5 dict dup begin\n/lenIV", "/lenIV"},
};

static bool RunMissingValueTokenCases() {
	const size_t count = sizeof(scMissingValueTokenCases) / sizeof(scMissingValueTokenCases[0]);
	for(size_t i = 0; i < count; ++i) {
		const MissingValueTokenCase& testCase = scMissingValueTokenCases[i];

		// Arrange
		string pfb = Type1SyntheticBuilder::RawPFBFromAsciiSegment(testCase.ascii);

		// Act
		Type1Input type1;
		EStatusCode status = Type1SyntheticBuilder::ParseAsType1(pfb, type1);

		// Assert
		if(status == eSuccess) {
			cout << "Type1InputTest [MissingValueToken::" << testCase.label
			     << "]: missing " << testCase.danglingKey
			     << " value was silently accepted" << endl;
			return false;
		}
	}
	return true;
}

// P3: Reset() did not default Type1FontDictionary::PaintType / FontType /
// FontBBox. A valid font that omits those keys left them indeterminate.
// Reset() now seeds them (PaintType 0, FontType 1, FontBBox all 0); a parse
// that never assigns them must surface exactly those values.
static bool Reset_OmittedFontDictMetrics_DefaultsApplied() {
	// Arrange: canned header minus /PaintType, /FontType, /FontBBox.
	const string headerOmittingMetrics =
		"%!PS-AdobeFont-1.0: Synth 001.000\n"
		"12 dict begin\n"
		"/FontInfo 4 dict dup begin\n"
		"/version (001.000) readonly def\n"
		"/FullName (Synth) readonly def\n"
		"/FamilyName (Synth) readonly def\n"
		"/Weight (Regular) readonly def\n"
		"end readonly def\n"
		"/FontName /Synth def\n"
		"/FontMatrix [0.001 0 0 0.001 0 0] readonly def\n"
		"/Encoding StandardEncoding def\n"
		"currentdict end\n"
		"currentfile eexec\n";
	vector<Type1SyntheticBuilder::NamedCharString> noGlyphs;
	string pfb = Type1SyntheticBuilder::WithCharStrings(noGlyphs, headerOmittingMetrics);

	Type1Input type1;
	// Act
	if(Type1SyntheticBuilder::ParseAsType1(pfb, type1) != eSuccess) {
		cout << "Type1InputTest [Reset::OmittedFontDictMetrics_DefaultsApplied]: "
		        "valid font omitting the metric keys failed to parse" << endl;
		return false;
	}

	// Assert (exact Reset() defaults)
	if(type1.mFontDictionary.PaintType != 0) {
		cout << "Type1InputTest [Reset::OmittedFontDictMetrics_DefaultsApplied]: "
		        "PaintType " << type1.mFontDictionary.PaintType << ", expected 0" << endl;
		return false;
	}
	if(type1.mFontDictionary.FontType != 1) {
		cout << "Type1InputTest [Reset::OmittedFontDictMetrics_DefaultsApplied]: "
		        "FontType " << type1.mFontDictionary.FontType << ", expected 1" << endl;
		return false;
	}
	for(int i = 0; i < 4; ++i) {
		if(type1.mFontDictionary.FontBBox[i] != 0) {
			cout << "Type1InputTest [Reset::OmittedFontDictMetrics_DefaultsApplied]: "
			        "FontBBox[" << i << "] " << type1.mFontDictionary.FontBBox[i]
			     << ", expected 0" << endl;
			return false;
		}
	}
	return true;
}

// V-072 regression guard: GetNextToken returns {false,""} at a PFB segment
// boundary (segment tail is whitespace) even though the next segment carries
// data. ReadNextTokenValue must retry across such a boundary, not reject the
// font. Here /PaintType is the last token of ASCII segment 1 (a trailing
// newline makes the value-read's GetNextToken hit the no-token boundary
// path); its value "7" is the first token of segment 2. The parse must
// succeed and PaintType must read as 7.
static bool ReadNextTokenValue_ValueAcrossSegmentBoundary_Succeeds() {
	// Arrange
	vector<string> headerSegments;
	headerSegments.push_back(
		"%!PS-AdobeFont-1.0: Synth 001.000\n"
		"12 dict begin\n"
		"/FontInfo 4 dict dup begin\n"
		"/version (001.000) readonly def\n"
		"/FullName (Synth) readonly def\n"
		"/FamilyName (Synth) readonly def\n"
		"/Weight (Regular) readonly def\n"
		"end readonly def\n"
		"/FontName /Synth def\n"
		"/FontType 1 def\n"
		"/FontMatrix [0.001 0 0 0.001 0 0] readonly def\n"
		"/FontBBox {0 0 1000 1000} readonly def\n"
		"/Encoding StandardEncoding def\n"
		"/PaintType\n\n");                     // key + one consumed terminator
		                                       // + one leftover whitespace, so
		                                       // the value-read GetNextToken
		                                       // hits the segment-end no-token
		                                       // path; value is in segment 2
	headerSegments.push_back(
		"7 def\n"
		"currentdict end\n"
		"currentfile eexec\n");
	vector<Type1SyntheticBuilder::NamedCharString> noGlyphs;
	string pfb = Type1SyntheticBuilder::WithCharStrings(noGlyphs, headerSegments);

	Type1Input type1;
	// Act
	EStatusCode status = Type1SyntheticBuilder::ParseAsType1(pfb, type1);

	// Assert
	if(status != eSuccess) {
		cout << "Type1InputTest [ReadNextTokenValue::ValueAcrossSegmentBoundary_Succeeds]: "
		        "valid font with /PaintType value in the next segment was rejected" << endl;
		return false;
	}
	if(type1.mFontDictionary.PaintType != 7) {
		cout << "Type1InputTest [ReadNextTokenValue::ValueAcrossSegmentBoundary_Succeeds]: "
		        "PaintType " << type1.mFontDictionary.PaintType << ", expected 7" << endl;
		return false;
	}
	return true;
}

int Type1InputTest(int argc, char* argv[]) {
	(void) argc;
	if(!ReadType1File_RealPFB_ParsesFontInfoStrings(argv)) return 1;
	if(!RunAddDependentGlyphsCases()) return 1;
	if(!RunMissingValueTokenCases()) return 1;
	if(!ReadNextTokenValue_ValueAcrossSegmentBoundary_Succeeds()) return 1;
	if(!Reset_OmittedFontDictMetrics_DefaultsApplied()) return 1;
	return 0;
}
