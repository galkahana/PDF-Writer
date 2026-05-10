/*
   Source File : CFFIndexSanity.cpp


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


   Regression test for the CFF INDEX offset-table sanity checks in
   ReadIndexHeader and ReadCharString. Each malformed CFF below would
   previously reach an unsigned subtraction on garbage offsets and feed the
   result to `new Byte[N]`, a bad_alloc / OOB primitive depending on the
   values. With validation in place, every malformed prefix is rejected
   with eFailure on the very first INDEX (the Name INDEX).

   Cases are grouped by the function they exercise (sc<Function>Cases) and
   driven by Run<Function>Cases runners. Each row's label states
   <Condition>_<ExpectedResult>; the runner prefixes the function name so
   failure messages stay readable. Synthetic streams come from
   CFFSyntheticBuilder; the happy-path test parses BrushScriptStd.otf and
   asserts exact values (font name, font count, .notdef glyph) so a
   regression that quietly turns the parser into a no-op would be caught.
*/
#include "CFFFileInput.h"
#include "CFFSyntheticBuilder.h"
#include "EStatusCode.h"
#include "InputFile.h"
#include "OpenTypeFileInput.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

struct ReadIndexHeaderCase {
	const char* mLabel;
	const char* mPayload;
	size_t mPayloadLen;
};

// Each row is a malformed Name INDEX that ReadIndexHeader must reject.
// Pre-fix, all four reached an unsigned subtraction on garbage offsets and
// fed it to `new Byte[N]`.
static const ReadIndexHeaderCase scReadIndexHeaderCases[] = {
	// offsets[0] < 1 would underflow `dataStartPosition + offsets[0] - 1`
	// in ReadSubrsFromIndex (and produce wild charstring start positions).
	{"ZeroFirstOffset_ReturnsFailure", CFF_BYTES("\x00\x01\x01\x00\x01")},
	// Non-monotonic offsets used to make `offsets[i+1] - offsets[i]` wrap
	// to ~ULONG_MAX in unsigned arithmetic, then drive `new Byte[N]`.
	{"NonMonotonicOffsets_ReturnsFailure", CFF_BYTES("\x00\x01\x01\x05\x01")},
	// offSize=0 leaves ReadOffset's switch with no matching case, returning
	// eFailure but with offset values uninitialized in the buffer.
	{"ZeroOffSize_ReturnsFailure", CFF_BYTES("\x00\x01\x00")},
	// offSize=5 is out of the spec-required {1..4} range.
	{"OutOfRangeOffSize_ReturnsFailure", CFF_BYTES("\x00\x01\x05")},
};

static bool RunReadIndexHeaderCases() {
	bool ok = true;
	const size_t caseCount = sizeof(scReadIndexHeaderCases) / sizeof(scReadIndexHeaderCases[0]);
	for(size_t i = 0; i < caseCount; ++i) {
		const ReadIndexHeaderCase& c = scReadIndexHeaderCases[i];

		// Arrange
		CFFFileInput cff;
		string bytes = CFFSyntheticBuilder::HeaderPlusBytes(c.mPayload, c.mPayloadLen);

		// Act
		EStatusCode status = CFFSyntheticBuilder::ParseAsCFF(bytes, cff);

		// Assert
		if(status == eSuccess) {
			cout << "CFFIndexSanity [ReadIndexHeader::" << c.mLabel
			     << "]: malformed input was accepted" << endl;
			ok = false;
		}
	}
	return ok;
}

// Parses BrushScriptStd.otf and leaves the file open through the caller's
// scope so the CFF reader's stream pointer stays valid for follow-up
// ReadCharString calls. The caller's outFile destructor closes the file.
static EStatusCode openBrushScriptStd(char* argv[], InputFile& outFile, OpenTypeFileInput& outOpenType) {
	if(outFile.OpenFile(BuildRelativeInputPath(argv, "fonts/BrushScriptStd.otf")) != eSuccess)
		return eFailure;
	return outOpenType.ReadOpenTypeFile(outFile.GetInputStream(), 0);
}

// ReadCharString with end < start would underflow the unsigned subtraction
// fed to `new Byte[N]`. Reachable directly via the public
// IType2InterpreterImplementation::ReadCharString override.
static bool ReadCharString_EndBeforeStart_ReturnsFailure(char* argv[]) {
	// Arrange: parse a valid font so mPrimitivesReader has a stream.
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(openBrushScriptStd(argv, otfFile, openType) != eSuccess) {
		cout << "CFFIndexSanity [ReadCharString::EndBeforeStart_ReturnsFailure]: BrushScriptStd.otf parse failed" << endl;
		return false;
	}

	// Act: pick legitimate-looking offsets but with end strictly less than start.
	Byte* charString = NULL;
	EStatusCode status = openType.mCFF.ReadCharString(/*start*/ 1000, /*end*/ 500, &charString);

	// Assert
	if(status == eSuccess) {
		delete[] charString;
		cout << "CFFIndexSanity [ReadCharString::EndBeforeStart_ReturnsFailure]: end < start was accepted" << endl;
		return false;
	}
	if(charString != NULL) {
		// Function contract: failure path leaves outCharString NULL (never allocated).
		delete[] charString;
		cout << "CFFIndexSanity [ReadCharString::EndBeforeStart_ReturnsFailure]: non-NULL buffer left on failure" << endl;
		return false;
	}
	return true;
}

// Happy path: prove the new validation didn't break parsing of a real CFF
// font. Asserts exact expected values rather than just "didn't crash" /
// "count > 0", so a regression that quietly turns the parser into a no-op
// (e.g. always returning empty data) would be caught.
static bool ReadCFFFile_BrushScriptStd_PopulatesExpectedValues(char* argv[]) {
	// Arrange + Act
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(openBrushScriptStd(argv, otfFile, openType) != eSuccess) {
		cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: real CFF rejected" << endl;
		return false;
	}

	// Assert
	bool ok = false;
	Byte* buffer = NULL;
	do {
		if(openType.mCFF.mFontsCount != 1) {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: expected 1 font, got "
			     << openType.mCFF.mFontsCount << endl;
			break;
		}
		if(openType.mCFF.mName.size() != 1) {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: expected 1 name entry, got "
			     << openType.mCFF.mName.size() << endl;
			break;
		}
		if(openType.mCFF.mName.front() != "BrushScriptStd") {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: expected font name 'BrushScriptStd', got '"
			     << openType.mCFF.mName.front() << "'" << endl;
			break;
		}
		if(openType.mCFF.GetCharStringsCount(0) == 0) {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: expected non-zero glyph count" << endl;
			break;
		}
		if(openType.mCFF.GetGlyphName(0, 0) != ".notdef") {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: expected glyph 0 name '.notdef', got '"
			     << openType.mCFF.GetGlyphName(0, 0) << "'" << endl;
			break;
		}

		// ReadCharString round-trips a real glyph (proves the end >= start
		// guard didn't accidentally reject the normal end == start + N case).
		CharString* notdef = openType.mCFF.GetGlyphCharString(0, 0);
		if(notdef == NULL) {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: GetGlyphCharString returned NULL for .notdef" << endl;
			break;
		}
		EStatusCode readStatus = openType.mCFF.ReadCharString(
			notdef->mStartPosition, notdef->mEndPosition, &buffer);
		if(readStatus != eSuccess) {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: ReadCharString failed on .notdef glyph" << endl;
			break;
		}
		if(buffer == NULL) {
			cout << "CFFIndexSanity [ReadCFFFile::BrushScriptStd_PopulatesExpectedValues]: ReadCharString reported success but left buffer NULL" << endl;
			break;
		}

		ok = true;
	} while(false);

	delete[] buffer;
	return ok;
}

int CFFIndexSanity(int argc, char* argv[]) {
	if(!RunReadIndexHeaderCases()) return 1;
	if(!ReadCharString_EndBeforeStart_ReturnsFailure(argv)) return 1;
	if(!ReadCFFFile_BrushScriptStd_PopulatesExpectedValues(argv)) return 1;
	return 0;
}
