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

int CFFFileInputTest(int argc, char* argv[]) {
	if(!RunGetSingleIntegerValueFromDictCases()) return 1;
	if(!RunReadPrivateDictCases()) return 1;
	if(!RunReadEncodingCases()) return 1;
	if(!RunReadFDSelectCases()) return 1;
	if(!ReadCFFFile_BrushScriptStd_PopulatesPrivateDict(argv)) return 1;
	return 0;
}
