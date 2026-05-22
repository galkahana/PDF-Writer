/*
   Source File : CFFFileInputTest.cpp


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


   Regression tests for CFFFileInput's input-validation paths:

     * V-041: GetSingleIntegerValueFromDict / ReadPrivateDict were calling
       front()/back() on the operand list with no checks, so a malformed Top
       DICT could feed UB / type-punned union reads into seek offsets and
       ReadDict's read amount.
     * V-031: ReadEncoding's format-1 path summed each range's `left` byte
       into a Byte mEncodingsCount that wrapped at 256, undersizing the
       allocation and overflowing the heap during the second-pass fill.
     * V-032: ReadEncoding's supplements branch seeked to an uninitialized
       mEncodingEnd before it had been assigned on the custom-encoding
       path, landing the cursor at a junk offset.
     * V-030: ReadFDSelect failed to bound fdIndex against the FDArray
       length and (for format 3) failed to bound nextRangeGlyphIndex
       against glyphCount, yielding a wild-pointer write or a heap write
       past the end of mFDSelect.
     * V-085: AddDependentGlyphs / CollectComponentGlyphs recursed on the
       Type 2 seac (4-arg endchar) dependency graph without a visited-set
       guard or a depth cap, so a self-referencing or cyclic seac chain
       drove the call stack until it overflowed.
     * V-044: the three charset format readers wrote (*inSIDArray)[0] = 0
       unconditionally, but a font that omits /CharStrings has
       mCharStringsCount == 0, making *inSIDArray a new unsigned short[0]
       zero-length buffer — element 0 is an out-of-bounds heap write.
     * V-048: ReadCharString deleted the charstring buffer on a failed read
       but left *outCharString dangling; every Type 2 interpreter call site
       then ran an unconditional delete[] on the same pointer (double-free)
       when the charstring data was truncated past the stream end.

   (V-046 — the format-0 encoding loop's Byte counter — was structurally
   closed by #364's V-031 widening, which rewrote that loop to use an
   unsigned short counter; no separate change or case here. V-047 —
   latent stale mCurrentCharsetInfo deref in Type2Endchar — is a
   constructor-init + NULL-guard hardening with no attacker-reachable
   trigger today, so it follows the latent-fix principle: no synthetic
   no-op case, covered by the existing suite.)

   Cases are grouped by the function they exercise (sc<Function>Cases) and
   driven by Run<Function>Cases runners. Each row's label states
   <Condition>_<ExpectedResult>; the runner prefixes the function name so
   failure messages stay readable.
*/
#include "CFFFileInput.h"
#include "CFFSyntheticBuilder.h"
#include "DictOperand.h"
#include "EStatusCode.h"
#include "InputByteArrayStream.h"
#include "InputFile.h"
#include "OpenTypeFileInput.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace PDFHummus;

typedef string (*CFFBuilderFn)(const char*, size_t);

// A row in a parameterised failure-case table: each row supplies a label
// plus the malformed payload bytes that should be rejected by the function
// under test (called via the table runner's bound builder + ParseAsCFF).
struct ParseFailureCase {
	const char* mLabel;
	const char* mPayload;
	size_t mPayloadLen;
};

// Row for tables where parsing must SUCCEED but the parser should have
// fallen back to a documented default (e.g. /CharStrings with a malformed
// operand list — GetSingleIntegerValueFromDict returns the default 0 and
// ReadCharStrings becomes a no-op).
struct ParseFallbackCase {
	const char* mLabel;
	const char* mPayload;
	size_t mPayloadLen;
	unsigned short mExpectedCharStringsCount;
};

// Shared inner loop for failure-case tables. Reports every unexpectedly-
// successful parse so a single test run shows all regressions, not just
// the first.
static bool ExpectAllParseFailures(const char* inFunctionName,
                                   CFFBuilderFn inBuild,
                                   const ParseFailureCase* inCases,
                                   size_t inCaseCount) {
	bool ok = true;
	for(size_t i = 0; i < inCaseCount; ++i) {
		// Arrange
		CFFFileInput cff;
		string bytes = inBuild(inCases[i].mPayload, inCases[i].mPayloadLen);

		// Act
		EStatusCode status = CFFSyntheticBuilder::ParseAsCFF(bytes, cff);

		// Assert
		if(status == eSuccess) {
			cout << "CFFFileInputTest [" << inFunctionName << "::" << inCases[i].mLabel
			     << "]: malformed input was accepted" << endl;
			ok = false;
		}
	}
	return ok;
}

// V-041: GetSingleIntegerValueFromDict ignored empty-list / non-integer /
// negative operands. Pre-fix, /CharStrings with any of these shapes fed
// garbage into SetOffset and ReadCard16. Post-fix the helper falls back to
// the supplied default (0), turning ReadCharStrings into a no-op.
static const ParseFallbackCase scGetSingleIntegerValueFromDictCases[] = {
	// Single operator, zero operands. Pre-fix did front().IntegerValue on
	// an empty list — UB.
	{"EmptyOperandList_FallsBackToDefault", CFF_BYTES("\x11"), 0},
	// Real where the spec requires an integer. IntegerValue was read from
	// the union over RealValue, yielding a platform-dependent bit pattern.
	{"RealOperand_FallsBackToDefault", CFF_BYTES("\x1E\x0A\x5F\x11"), 0},
	// Negative integer would have driven SetOffset(negative) and failed
	// the seek; the helper now rejects negatives.
	{"NegativeOperand_FallsBackToDefault", CFF_BYTES("\x1C\xFF\xFF\x11"), 0},
};

static bool RunGetSingleIntegerValueFromDictCases() {
	bool ok = true;
	const size_t caseCount = sizeof(scGetSingleIntegerValueFromDictCases) /
	                         sizeof(scGetSingleIntegerValueFromDictCases[0]);
	for(size_t i = 0; i < caseCount; ++i) {
		const ParseFallbackCase& c = scGetSingleIntegerValueFromDictCases[i];

		// Arrange
		CFFFileInput cff;
		string bytes = CFFSyntheticBuilder::TopDictOnly(c.mPayload, c.mPayloadLen);

		// Act
		EStatusCode status = CFFSyntheticBuilder::ParseAsCFF(bytes, cff);

		// Assert
		if(status != eSuccess) {
			cout << "CFFFileInputTest [GetSingleIntegerValueFromDict::" << c.mLabel
			     << "]: parse failed; expected fallback to default" << endl;
			ok = false;
			continue;
		}
		if(cff.GetCharStringsCount(0) != c.mExpectedCharStringsCount) {
			cout << "CFFFileInputTest [GetSingleIntegerValueFromDict::" << c.mLabel
			     << "]: charstrings count = " << cff.GetCharStringsCount(0)
			     << ", expected " << c.mExpectedCharStringsCount << endl;
			ok = false;
		}
	}
	return ok;
}

// V-041: ReadPrivateDict accepted operand lists that violated the spec's
// (size, offset) shape. Each row used to flow garbage into SetOffset /
// ReadDict's read amount; post-fix every shape is rejected with eFailure.
static const ParseFailureCase scReadPrivateDictCases[] = {
	// /Private (key 18) with no operands — front()/back() on empty list
	// were UB.
	{"EmptyOperandList_ReturnsFailure", CFF_BYTES("\x12")},
	// /Private with one integer — front()==back() so size and offset
	// became the same value; with operand 0 ReadDict silently parsed zero
	// bytes and ReadCFFFile returned success.
	{"SingleIntegerOperand_ReturnsFailure", CFF_BYTES("\x8B\x12")},
	// /Private with a real where the spec requires an integer —
	// IntegerValue read from the union over RealValue.
	{"RealOperand_ReturnsFailure", CFF_BYTES("\x1E\x0A\x5F\x8B\x12")},
	// /Private with a negative size would convert to a huge unsigned value
	// when fed to ReadDict's unsigned read amount.
	{"NegativeSizeOperand_ReturnsFailure", CFF_BYTES("\x1C\xFF\xFF\x8B\x12")},
};

static bool RunReadPrivateDictCases() {
	return ExpectAllParseFailures("ReadPrivateDict",
	                              CFFSyntheticBuilder::TopDictOnly,
	                              scReadPrivateDictCases,
	                              sizeof(scReadPrivateDictCases) / sizeof(scReadPrivateDictCases[0]));
}

// V-031: ReadEncoding's format-1 path summed each range's `left` field into
// a Byte mEncodingsCount that wrapped at 256, undersizing the allocation
// and overflowing the heap during the second-pass fill loop. Post-fix the
// sum is computed in unsigned short and rejected if it exceeds the
// 256-code spec ceiling.
static const ParseFailureCase scReadEncodingCases[] = {
	// format=0x01, ranges=2, each covers 255 codes (sum=510 > 256).
	{"Format1RangeSumExceedsCeiling_ReturnsFailure", CFF_BYTES("\x01\x02\x00\xFF\x01\xFF")},
};

// Format-1 happy path: a single range [first=0x40, left=5] should populate
// mEncoding[0..4] with codes 0x40..0x44. Catches a regression where the
// widening of mEncodingsCount accidentally broke the second-pass fill loop.
static bool ReadEncoding_Format1ValidRange_PopulatesEncodingArray() {
	// Arrange: format=0x01, ranges=1, range0={first=0x40, left=5}.
	CFFFileInput cff;
	string bytes = CFFSyntheticBuilder::WithEncoding(CFF_BYTES("\x01\x01\x40\x05"));

	// Act
	EStatusCode status = CFFSyntheticBuilder::ParseAsCFF(bytes, cff);

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest [ReadEncoding::Format1ValidRange_PopulatesEncodingArray]: parse failed" << endl;
		return false;
	}
	const EncodingsInfo* enc = cff.mTopDictIndex[0].mEncoding;
	if(enc == NULL) {
		cout << "CFFFileInputTest [ReadEncoding::Format1ValidRange_PopulatesEncodingArray]: mEncoding is NULL" << endl;
		return false;
	}
	if(enc->mEncodingsCount != 5) {
		cout << "CFFFileInputTest [ReadEncoding::Format1ValidRange_PopulatesEncodingArray]: encodings count = "
		     << enc->mEncodingsCount << ", expected 5" << endl;
		return false;
	}
	for(unsigned short i = 0; i < 5; ++i) {
		IOBasicTypes::Byte expected = (IOBasicTypes::Byte)(0x40 + i);
		if(enc->mEncoding[i] != expected) {
			cout << "CFFFileInputTest [ReadEncoding::Format1ValidRange_PopulatesEncodingArray]: encoding["
			     << i << "] = " << (int)enc->mEncoding[i] << ", expected " << (int)expected << endl;
			return false;
		}
	}
	return true;
}

// V-032: supplements branch seeked to an uninitialized mEncodingEnd before
// it was assigned on the custom-encoding path. Pre-fix the seek landed at
// junk and mSupplements never got the (encoding, SID) pair from the file.
// Post-fix the cursor is already at the supplements bytes when the branch
// runs, so the seek is removed and the entries land where they belong.
static bool ReadEncoding_Format0WithSupplements_PopulatesSupplementMap() {
	// Arrange: format byte 0x80 = format-0 base + supplements flag,
	// format-0 body rawCount=0, supplements count=1,
	// entry={encoding=0x42, SID=0x0001}.
	CFFFileInput cff;
	string bytes = CFFSyntheticBuilder::WithEncoding(CFF_BYTES("\x80\x00\x01\x42\x00\x01"));

	// Act
	EStatusCode status = CFFSyntheticBuilder::ParseAsCFF(bytes, cff);

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest [ReadEncoding::Format0WithSupplements_PopulatesSupplementMap]: parse failed" << endl;
		return false;
	}
	const EncodingsInfo* enc = cff.mTopDictIndex[0].mEncoding;
	if(enc == NULL) {
		cout << "CFFFileInputTest [ReadEncoding::Format0WithSupplements_PopulatesSupplementMap]: mEncoding is NULL" << endl;
		return false;
	}
	if(enc->mEncodingsCount != 0) {
		cout << "CFFFileInputTest [ReadEncoding::Format0WithSupplements_PopulatesSupplementMap]: base encoding count = "
		     << enc->mEncodingsCount << ", expected 0" << endl;
		return false;
	}
	UShortToByteList::const_iterator it = enc->mSupplements.find(1);
	if(it == enc->mSupplements.end()) {
		cout << "CFFFileInputTest [ReadEncoding::Format0WithSupplements_PopulatesSupplementMap]: SID 1 not in supplements map" << endl;
		return false;
	}
	if(it->second.size() != 1 || it->second.front() != 0x42) {
		cout << "CFFFileInputTest [ReadEncoding::Format0WithSupplements_PopulatesSupplementMap]: supplements[SID 1] mismatch" << endl;
		return false;
	}
	return true;
}

// The single-font ReadEncodings(unsigned short) overload (driven by the
// by-index / by-name ReadCFFFile entry points) used to call ReadEncoding
// and discard its return, then report mPrimitivesReader.GetInternalState().
// V-031's reject path returns eFailure without flipping the primitive
// reader's state, so the malformed encoding was accepted on those entries.
// This case feeds the same payload as the format-1-overflow row through
// ReadCFFFile(stream, fontIndex=0) so the fix is exercised on the by-index
// path too.
static bool ReadEncodings_ByFontIndex_PropagatesEncodingFailure() {
	// Arrange
	string bytes = CFFSyntheticBuilder::WithEncoding(CFF_BYTES("\x01\x02\x00\xFF\x01\xFF"));
	InputByteArrayStream stream((IOBasicTypes::Byte*)bytes.data(),
	                            (LongFilePositionType)bytes.size());
	CFFFileInput cff;

	// Act
	EStatusCode status = cff.ReadCFFFile(&stream, (unsigned short)0);

	// Assert
	if(status == eSuccess) {
		cout << "CFFFileInputTest [ReadEncodings::ByFontIndex_PropagatesEncodingFailure]: "
		        "malformed format-1 encoding accepted by by-index ReadCFFFile" << endl;
		return false;
	}
	return true;
}

static bool RunReadEncodingCases() {
	if(!ExpectAllParseFailures("ReadEncoding",
	                           CFFSyntheticBuilder::WithEncoding,
	                           scReadEncodingCases,
	                           sizeof(scReadEncodingCases) / sizeof(scReadEncodingCases[0])))
		return false;
	if(!ReadEncoding_Format1ValidRange_PopulatesEncodingArray()) return false;
	if(!ReadEncoding_Format0WithSupplements_PopulatesSupplementMap()) return false;
	if(!ReadEncodings_ByFontIndex_PropagatesEncodingFailure()) return false;
	return true;
}

// V-030: ReadFDSelect didn't bound fdIndex against the FDArray entry count
// (wild-pointer write into mFDSelect) and format-3 didn't enforce coverage
// of [0, glyphCount) (uninitialized mFDSelect entries dereferenced by
// later glyph-interpretation lookups). CFFSyntheticBuilder::WithFDSelect
// produces a CID font with glyphCount=1 and fdArrayCount=1 so each
// malformed-input row is a single payload.
static const ParseFailureCase scReadFDSelectCases[] = {
	// Format 0, fdIndex[0]=0xFF — pre-fix stored `mFDArray + 0xFF` in
	// mFDSelect[0] (wild pointer write).
	{"Format0FdIndexBeyondFDArray_ReturnsFailure", CFF_BYTES("\x00\xFF")},
	// Format 3, range fdIndex=0xFF — same wild-pointer-write primitive on
	// the format-3 read path.
	{"Format3FdIndexBeyondFDArray_ReturnsFailure", CFF_BYTES("\x03\x00\x01\x00\x00\xFF\x00\x01")},
	// Format 3, range covers glyphs [0, 5) but glyphCount is 1 — pre-fix
	// inner loop wrote mFDSelect[1..4] past the end of the heap buffer.
	{"Format3NextRangeBeyondGlyphCount_ReturnsFailure", CFF_BYTES("\x03\x00\x01\x00\x00\x00\x00\x05")},
	// Format 3 with rangesCount=0, firstGlyphIndex=1 — leaves mFDSelect[0]
	// uninitialized for later deref. Caught by the initial-firstGlyphIndex
	// check (must be 0 so every glyph is covered).
	{"Format3InitialFirstGlyphIndexNonZero_ReturnsFailure", CFF_BYTES("\x03\x00\x00\x00\x01")},
	// Format 3 with rangesCount=0, firstGlyphIndex=0 — loop never assigns
	// anything, so the trailing-coverage check fires because the final
	// sentinel (still 0) is less than glyphCount (1).
	{"Format3FinalSentinelBelowGlyphCount_ReturnsFailure", CFF_BYTES("\x03\x00\x00\x00\x00")},
};

static bool RunReadFDSelectCases() {
	return ExpectAllParseFailures("ReadFDSelect",
	                              CFFSyntheticBuilder::WithFDSelect,
	                              scReadFDSelectCases,
	                              sizeof(scReadFDSelectCases) / sizeof(scReadFDSelectCases[0]));
}

// Real-font happy path: prove the new validation didn't break parsing of a
// real CFF font. Asserts exact expected values rather than just "didn't
// crash" / "size > 0", so a regression that quietly turns the parser into
// a no-op would be caught.
static bool ReadCFFFile_BrushScriptStd_PopulatesPrivateDict(char* argv[]) {
	// Arrange
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(otfFile.OpenFile(BuildRelativeInputPath(argv, "fonts/BrushScriptStd.otf")) != eSuccess) {
		cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: failed to open font" << endl;
		return false;
	}

	// Act
	if(openType.ReadOpenTypeFile(otfFile.GetInputStream(), 0) != eSuccess) {
		cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: real CFF rejected" << endl;
		return false;
	}

	// Assert
	bool ok = false;
	do {
		if(openType.mCFF.mFontsCount != 1) {
			cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: expected 1 font, got "
			     << openType.mCFF.mFontsCount << endl;
			break;
		}
		if(openType.mCFF.mPrivateDicts == NULL) {
			cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: mPrivateDicts is NULL" << endl;
			break;
		}
		const PrivateDictInfo& priv = openType.mCFF.mPrivateDicts[0];
		// Exact byte positions for BrushScriptStd's Private DICT (offset=17676, size=28).
		if(priv.mPrivateDictStart != 17676) {
			cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: Private DICT start "
			     << priv.mPrivateDictStart << ", expected 17676" << endl;
			break;
		}
		if(priv.mPrivateDictEnd != 17704) {
			cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: Private DICT end "
			     << priv.mPrivateDictEnd << ", expected 17704" << endl;
			break;
		}
		if(priv.mPrivateDict.empty()) {
			cout << "CFFFileInputTest [ReadCFFFile::BrushScriptStd_PopulatesPrivateDict]: inner Private DICT empty" << endl;
			break;
		}
		ok = true;
	} while(false);

	return ok;
}

// V-085 helpers
//
// CFFSyntheticBuilder::WithCharStrings produces a non-CID CFF with the
// ISOAdobe charset (predefined offset 0), so glyph K is assigned SID K =
// scStandardStrings[K]. StandardEncoding maps code (32 + K - 1) to that
// same standard string for K in [1..95], so a Type 2 4-arg endchar with
// bchar = achar = (K + 31) creates a dependency from the issuing glyph
// to glyph K. That gives us a way to express any directed-graph between
// glyphs 1..N in self-referencing fixtures.

// Build a 5-byte Type 2 seac-flavored endchar referencing standard
// encoding code inCode for both bchar and achar: "0 0 code code endchar".
// Codes <= 107 encode as a single CFF Type 2 integer byte (code + 139).
static string MakeSelfEndcharReferencingCode(IOBasicTypes::Byte inCode) {
	const char one = (char)((int)inCode + 139);
	char bytes[] = { '\x8B', '\x8B', one, one, '\x0E' };
	return string(bytes, sizeof(bytes));
}

// Helper to query whether a glyph ID survived the AddDependentGlyphs walk.
static bool ContainsGlyph(const std::vector<unsigned int>& inGlyphs, unsigned int inID) {
	for(size_t i = 0; i < inGlyphs.size(); ++i)
		if(inGlyphs[i] == inID) return true;
	return false;
}

// Parses inCFFBytes into outCFF AND keeps the InputByteArrayStream alive in
// outStream for as long as the caller needs to re-enter the parser (e.g.
// AddDependentGlyphs / CalculateDependenciesForCharIndex run the Type 2
// interpreter, which calls back into CFFFileInput::ReadCharString and seeks
// the underlying stream). ParseAsCFF's stream is local to that call, so it
// can't be used by tests that interact with the parser after the load.
static EStatusCode ParseKeepingStream(const string& inCFFBytes,
                                      InputByteArrayStream& outStream,
                                      CFFFileInput& outCFF) {
	outStream.Assign((IOBasicTypes::Byte*)inCFFBytes.data(),
	                 (LongFilePositionType)inCFFBytes.size());
	return outCFF.ReadCFFFile(&outStream);
}

// V-085: glyph 1's CharString seac-refs standard encoding code 32 ("space"),
// which resolves via ISOAdobe charset back to glyph 1. Pre-fix the recursion
// had no visited-set guard, so AddDependentGlyphs blew the call stack on
// this single-glyph self-reference.
static bool AddDependentGlyphs_SelfReferencingSeac_TerminatesWithSelfDependency() {
	// Arrange: 1 issued glyph (glyph 1) whose CharString seac-references
	// standard encoding code 32 -> "space" -> SID 1 -> glyph 1.
	std::vector<std::string> charStrings;
	charStrings.push_back(MakeSelfEndcharReferencingCode(32));
	string bytes = CFFSyntheticBuilder::WithCharStrings(charStrings);

	InputByteArrayStream stream;
	CFFFileInput cff;
	if(ParseKeepingStream(bytes, stream, cff) != eSuccess) {
		cout << "CFFFileInputTest [AddDependentGlyphs::SelfReferencingSeac_TerminatesWithSelfDependency]: synthetic CFF parse failed" << endl;
		return false;
	}

	// Act
	std::vector<unsigned int> subset;
	subset.push_back(1);
	EStatusCode status = cff.AddDependentGlyphs(subset);

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest [AddDependentGlyphs::SelfReferencingSeac_TerminatesWithSelfDependency]: status not eSuccess" << endl;
		return false;
	}
	if(subset.size() != 1 || subset[0] != 1) {
		cout << "CFFFileInputTest [AddDependentGlyphs::SelfReferencingSeac_TerminatesWithSelfDependency]: subset != {1}" << endl;
		return false;
	}
	return true;
}

// V-085: two glyphs whose seac dependencies form a cycle (1 -> 2, 2 -> 1).
// Pre-fix the recursion ping-ponged between them until the stack overflowed.
static bool AddDependentGlyphs_TwoGlyphCycle_TerminatesWithBothDependencies() {
	// Arrange: glyph 1 references code 33 ("exclam" -> SID 2 -> glyph 2),
	// glyph 2 references code 32 ("space" -> SID 1 -> glyph 1).
	std::vector<std::string> charStrings;
	charStrings.push_back(MakeSelfEndcharReferencingCode(33));
	charStrings.push_back(MakeSelfEndcharReferencingCode(32));
	string bytes = CFFSyntheticBuilder::WithCharStrings(charStrings);

	InputByteArrayStream stream;
	CFFFileInput cff;
	if(ParseKeepingStream(bytes, stream, cff) != eSuccess) {
		cout << "CFFFileInputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: synthetic CFF parse failed" << endl;
		return false;
	}

	// Act
	std::vector<unsigned int> subset;
	subset.push_back(1);
	EStatusCode status = cff.AddDependentGlyphs(subset);

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: status not eSuccess" << endl;
		return false;
	}
	if(subset.size() != 2 || subset[0] != 1 || subset[1] != 2) {
		cout << "CFFFileInputTest [AddDependentGlyphs::TwoGlyphCycle_TerminatesWithBothDependencies]: subset != {1, 2}" << endl;
		return false;
	}
	return true;
}

// V-085: a 25-deep acyclic seac chain that would push the call stack past
// reasonable limits without a depth cap. The cap stops the walk before
// the tail glyph is reached. The test asserts the tail wasn't reached
// without baking the exact cap value into the assertion so future tuning
// of scMaxCompositeDepth doesn't break this test.
static bool AddDependentGlyphs_DeepAcyclicChain_StopsBeforeChainEnd() {
	// Arrange: glyph K (K=1..24) seac-refs standard encoding code (K+32),
	// which resolves to glyph K+1. Glyph 25 is a plain endchar (no seac).
	const unsigned int chainLength = 25;
	std::vector<std::string> charStrings;
	for(unsigned int k = 1; k < chainLength; ++k)
		charStrings.push_back(MakeSelfEndcharReferencingCode((IOBasicTypes::Byte)(k + 32)));
	charStrings.push_back(string("\x0E", 1));

	string bytes = CFFSyntheticBuilder::WithCharStrings(charStrings);
	InputByteArrayStream stream;
	CFFFileInput cff;
	if(ParseKeepingStream(bytes, stream, cff) != eSuccess) {
		cout << "CFFFileInputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: synthetic CFF parse failed" << endl;
		return false;
	}

	// Act
	std::vector<unsigned int> subset;
	subset.push_back(1);
	EStatusCode status = cff.AddDependentGlyphs(subset);

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: status not eSuccess" << endl;
		return false;
	}
	if(!ContainsGlyph(subset, 1)) {
		cout << "CFFFileInputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: input glyph 1 missing from subset" << endl;
		return false;
	}
	if(ContainsGlyph(subset, chainLength)) {
		cout << "CFFFileInputTest [AddDependentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: tail glyph "
		     << chainLength << " was reached; depth cap did not fire" << endl;
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

// V-038: Type2Endchar (deprecated seac flavor) cast both bchar / achar
// operands to Byte unconditionally, silently truncating any out-of-range
// value into [0, 255] and dispatching to the wrong StandardEncoding glyph.
// Post-fix the bounds check rejects out-of-range operands before narrowing.
//
// Each case feeds a synthetic 1-glyph CFF whose CharString is "0 0 bchar
// achar endchar" with one of bchar/achar encoded as Type 2 integer 288.
// 288 is out of [0, 255], but pre-fix (Byte)288 == 32, which StandardEncoding
// maps to "space" → SID 1 → glyph 1 (the issuing glyph itself in the
// synthetic ISOAdobe charset). So pre-fix the lookup found valid glyphs and
// Type2Endchar returned eSuccess; post-fix the bounds check fires and
// AddDependentGlyphs surfaces eFailure. This is the discriminating shape —
// the prior in-process variant returned eFailure on both pre- and post-fix
// code because mCurrentCharsetInfo was NULL.
//
// Type 2 integer encoding (Tech Note #5177):
//   single byte b in [32,246]  -> value = b - 139         range [-107, 107]
//   two bytes b0 in [247,250]  -> value = (b0-247)*256 + b1 + 108
//                                                          range [108, 1131]
// 32   -> 0xAB    (32 + 139)
// 288  -> 0xF7 0xB4    ((247-247)*256 + 180 + 108 = 288)
struct Type2EndcharOOBCase {
	const char* mLabel;
	const char* mCharString;
	size_t      mCharStringLen;
};

#define CSTR_BYTES(LIT) (LIT), (sizeof(LIT) - 1)

static const Type2EndcharOOBCase scType2EndcharOOBCases[] = {
	// 4 operands "0 0 bchar=288 achar=32 endchar": bchar OOB, achar in range.
	{"BcharIntegerAbove255_ReturnsFailure", CSTR_BYTES("\x8B\x8B\xF7\xB4\xAB\x0E")},
	// 4 operands "0 0 bchar=32 achar=288 endchar": achar OOB.
	{"AcharIntegerAbove255_ReturnsFailure", CSTR_BYTES("\x8B\x8B\xAB\xF7\xB4\x0E")},
};

static bool RunType2EndcharOOBCases() {
	const size_t count = sizeof(scType2EndcharOOBCases) / sizeof(scType2EndcharOOBCases[0]);
	for(size_t i = 0; i < count; ++i) {
		const Type2EndcharOOBCase& testCase = scType2EndcharOOBCases[i];

		// Arrange: synthetic 1-glyph CFF whose only CharString is the seac.
		std::vector<std::string> charStrings;
		charStrings.push_back(std::string(testCase.mCharString, testCase.mCharStringLen));
		std::string bytes = CFFSyntheticBuilder::WithCharStrings(charStrings);

		InputByteArrayStream stream;
		CFFFileInput cff;
		if(ParseKeepingStream(bytes, stream, cff) != eSuccess) {
			cout << "CFFFileInputTest [Type2Endchar::" << testCase.mLabel
			     << "]: synthetic CFF parse failed" << endl;
			return false;
		}

		// Act
		std::vector<unsigned int> subset;
		subset.push_back(1);
		EStatusCode status = cff.AddDependentGlyphs(subset);

		// Assert: pre-fix the truncation steered both seac codes to a real
		// glyph and AddDependentGlyphs returned eSuccess. Post-fix bounds
		// check fires inside Type2Endchar and the failure propagates.
		if(status == eSuccess) {
			cout << "CFFFileInputTest [Type2Endchar::" << testCase.mLabel
			     << "]: out-of-range seac operand silently accepted "
			        "(pre-fix truncated to a valid in-range code)" << endl;
			return false;
		}
	}
	return true;
}

// V-044: each charset format reader did (*inSIDArray)[0] = 0 before
// checking mCharStringsCount. A font that omits /CharStrings has
// mCharStringsCount == 0, so *inSIDArray is a new unsigned short[0]
// zero-length buffer and element 0 is an out-of-bounds heap write.
// WithCharset builds exactly that font (custom /charset, no /CharStrings);
// the only payload needed is the one-byte charset format selector, since
// with zero glyphs the per-glyph fill loops never iterate. Post-fix the
// parse still succeeds (an empty CharStrings INDEX is valid CFF) and a
// custom charset object is recorded.
struct CharsetFormatCase {
	const char* mLabel;
	const char* mPayload;
	size_t mPayloadLen;
};

static const CharsetFormatCase scReadCharsetCases[] = {
	{"Format0EmptyCharStrings_ParsesWithoutOOBWrite", CFF_BYTES("\x00")},
	{"Format1EmptyCharStrings_ParsesWithoutOOBWrite", CFF_BYTES("\x01")},
	{"Format2EmptyCharStrings_ParsesWithoutOOBWrite", CFF_BYTES("\x02")},
};

static bool RunReadCharsetCases() {
	bool ok = true;
	const size_t caseCount = sizeof(scReadCharsetCases) / sizeof(scReadCharsetCases[0]);
	for(size_t i = 0; i < caseCount; ++i) {
		const CharsetFormatCase& c = scReadCharsetCases[i];

		// Arrange
		CFFFileInput cff;
		string bytes = CFFSyntheticBuilder::WithCharset(c.mPayload, c.mPayloadLen);

		// Act
		EStatusCode status = CFFSyntheticBuilder::ParseAsCFF(bytes, cff);

		// Assert
		if(status != eSuccess) {
			cout << "CFFFileInputTest [ReadCharset::" << c.mLabel
			     << "]: parse failed; empty-CharStrings custom charset is valid CFF" << endl;
			ok = false;
			continue;
		}
		if(cff.GetCharStringsCount(0) != 0) {
			cout << "CFFFileInputTest [ReadCharset::" << c.mLabel
			     << "]: precondition broken — charstrings count = "
			     << cff.GetCharStringsCount(0) << ", expected 0" << endl;
			ok = false;
			continue;
		}
		const CharSetInfo* charSet = cff.mTopDictIndex[0].mCharSet;
		if(charSet == NULL || charSet->mType != eCharSetCustom) {
			cout << "CFFFileInputTest [ReadCharset::" << c.mLabel
			     << "]: custom charset not recorded" << endl;
			ok = false;
		}
	}
	return ok;
}

// V-048: ReadCharString allocated *outCharString, and on a failed read
// deleted it but left the caller's pointer dangling. Every Type 2
// interpreter call site (CharStringType2Interpreter::Intepret /
// InterpretCallSubr / InterpretCallGSubr) then ran an unconditional
// delete[] on that same pointer -> double-free. The CharStrings INDEX
// offset table is parsed up front but the charstring bytes are read
// lazily during interpretation, so a buffer truncated after the offset
// table still parses, then fails the read inside ReadCharString.
static bool ReadCharString_TruncatedCharStringData_NoDoubleFree() {
	// Arrange: one glyph with a 4-byte CharString, then drop the last 2
	// bytes. The offset table still claims 4 bytes for glyph 1, so the
	// snapshot positions are intact but the data is short by 2.
	std::vector<std::string> charStrings;
	charStrings.push_back(string("\x8B\x8B\x8B\x0E", 4));
	string bytes = CFFSyntheticBuilder::WithCharStrings(charStrings);
	if(bytes.size() < 2) {
		cout << "CFFFileInputTest [ReadCharString::TruncatedCharStringData_NoDoubleFree]: builder produced empty CFF" << endl;
		return false;
	}
	bytes.resize(bytes.size() - 2);

	InputByteArrayStream stream;
	CFFFileInput cff;
	if(ParseKeepingStream(bytes, stream, cff) != eSuccess) {
		cout << "CFFFileInputTest [ReadCharString::TruncatedCharStringData_NoDoubleFree]: truncated CFF unexpectedly rejected at parse time" << endl;
		return false;
	}

	// Act: interpreting glyph 1 reads its (truncated) charstring; pre-fix
	// this double-freed the buffer and aborted the process.
	std::vector<unsigned int> subset;
	subset.push_back(1);
	EStatusCode status = cff.AddDependentGlyphs(subset);

	// Assert: the read failure must surface as eFailure, and reaching this
	// line at all means there was no double-free abort.
	if(status == eSuccess) {
		cout << "CFFFileInputTest [ReadCharString::TruncatedCharStringData_NoDoubleFree]: "
		        "truncated charstring read reported success" << endl;
		return false;
	}
	return true;
}

static bool RunReadCharStringCases() {
	if(!ReadCharString_TruncatedCharStringData_NoDoubleFree()) return false;
	return true;
}

int CFFFileInputTest(int argc, char* argv[]) {
	if(!RunGetSingleIntegerValueFromDictCases()) return 1;
	if(!RunReadPrivateDictCases()) return 1;
	if(!RunReadEncodingCases()) return 1;
	if(!RunReadFDSelectCases()) return 1;
	if(!RunReadCharsetCases()) return 1;
	if(!RunReadCharStringCases()) return 1;
	if(!RunAddDependentGlyphsCases()) return 1;
	if(!RunType2EndcharOOBCases()) return 1;
	if(!ReadCFFFile_BrushScriptStd_PopulatesPrivateDict(argv)) return 1;
	return 0;
}
