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
   values. With validation in place, every malformed prefix is rejected with
   eFailure on the very first INDEX (the Name INDEX).

   The malformed streams are synthesised in-process so no binary fixtures
   are required. The happy-path test parses BrushScriptStd.otf and asserts
   exact values (font name, font count, .notdef glyph) so we can tell that
   the validation didn't accidentally turn the parser into a no-op.
*/
#include "InputByteArrayStream.h"
#include "InputFile.h"
#include "OpenTypeFileInput.h"
#include "CFFFileInput.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Header: major=1, minor=0, hdrSize=4, absOffSize=1. ReadCFFFile only
// uses hdrSize to skip to the Name INDEX, which is already at offset 4.
static const char scCFFHeader[] = "\x01\x00\x04\x01";
static const size_t scCFFHeaderSize = sizeof(scCFFHeader) - 1;

// Drive ReadCFFFile against a 4-byte header + the caller-crafted Name
// INDEX bytes. The Name INDEX always-fails cases never reach the other
// INDEXes, so we can stop the synthetic stream right after it.
static EStatusCode parseSyntheticCFF(const char* inNameIndexBytes, size_t inNameIndexLen) {
	string cff;
	cff.append(scCFFHeader, scCFFHeaderSize);
	cff.append(inNameIndexBytes, inNameIndexLen);
	InputByteArrayStream stream((Byte*)&cff[0], (LongFilePositionType)cff.size());
	CFFFileInput cffInput;
	return cffInput.ReadCFFFile(&stream);
}

// Pass a raw byte literal to parseSyntheticCFF, deriving the length from
// the literal so embedded \x00 bytes don't truncate the string. Use only
// with string literals — sizeof on a pointer would silently take 8 bytes.
#define PARSE_NAME_INDEX(bytes) parseSyntheticCFF((bytes), sizeof(bytes) - 1)

// offsets[0] < 1 would underflow `dataStartPosition + offsets[0] - 1`
// in ReadSubrsFromIndex (and produce wild charstring start positions).
static bool parsingZeroFirstOffset_returnsFailure() {
	// Arrange: count=1, offSize=1, offsets=[0x00, 0x01]
	EStatusCode status = PARSE_NAME_INDEX("\x00\x01\x01\x00\x01");

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: zero first offset was accepted" << endl;
		return false;
	}
	return true;
}

// Non-monotonic offsets used to make `offsets[i+1] - offsets[i]` wrap
// to ~ULONG_MAX in unsigned arithmetic, then drive `new Byte[N]`.
static bool parsingNonMonotonicOffsets_returnsFailure() {
	// Arrange: count=1, offSize=1, offsets=[0x05, 0x01] (1 < 5 -> non-monotonic)
	EStatusCode status = PARSE_NAME_INDEX("\x00\x01\x01\x05\x01");

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: non-monotonic offsets were accepted" << endl;
		return false;
	}
	return true;
}

// offSize=0 leaves ReadOffset's switch with no matching case, returning
// eFailure but with offset values uninitialized in the buffer.
static bool parsingZeroOffSize_returnsFailure() {
	// Arrange: count=1, offSize=0
	EStatusCode status = PARSE_NAME_INDEX("\x00\x01\x00");

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: offSize=0 was accepted" << endl;
		return false;
	}
	return true;
}

// Same with an out-of-range high value.
static bool parsingOutOfRangeOffSize_returnsFailure() {
	// Arrange: count=1, offSize=5
	EStatusCode status = PARSE_NAME_INDEX("\x00\x01\x05");

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: offSize=5 was accepted" << endl;
		return false;
	}
	return true;
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
static bool readCharStringWithEndBeforeStart_returnsFailure(char* argv[]) {
	// Arrange: parse a valid font so mPrimitivesReader has a stream.
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(openBrushScriptStd(argv, otfFile, openType) != eSuccess) {
		cout << "CFFIndexSanity: BrushScriptStd.otf parse failed" << endl;
		return false;
	}

	// Act: pick legitimate-looking offsets but with end strictly less than start.
	Byte* charString = NULL;
	EStatusCode status = openType.mCFF.ReadCharString(/*start*/ 1000, /*end*/ 500, &charString);

	// Assert
	if(status == eSuccess) {
		delete[] charString;
		cout << "CFFIndexSanity: ReadCharString accepted end < start" << endl;
		return false;
	}
	if(charString != NULL) {
		// Function contract: failure path leaves outCharString NULL (never allocated).
		delete[] charString;
		cout << "CFFIndexSanity: ReadCharString left non-NULL buffer on failure" << endl;
		return false;
	}
	return true;
}

// Happy path: prove the new validation didn't break parsing of a real CFF
// font. Asserts exact expected values rather than just "didn't crash" /
// "count > 0", so a regression that quietly turns the parser into a no-op
// (e.g. always returning empty data) would be caught.
static bool parsingValidCFF_populatesExpectedValues(char* argv[]) {
	// Arrange
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(openBrushScriptStd(argv, otfFile, openType) != eSuccess) {
		cout << "CFFIndexSanity: positive path failed - real CFF rejected" << endl;
		return false;
	}

	bool ok = false;
	Byte* buffer = NULL;

	do {
		// Assert: Name INDEX produced exactly one font with the expected name.
		if(openType.mCFF.mFontsCount != 1) {
			cout << "CFFIndexSanity: expected 1 font, got " << openType.mCFF.mFontsCount << endl;
			break;
		}
		if(openType.mCFF.mName.size() != 1) {
			cout << "CFFIndexSanity: expected 1 name entry, got " << openType.mCFF.mName.size() << endl;
			break;
		}
		if(openType.mCFF.mName.front() != "BrushScriptStd") {
			cout << "CFFIndexSanity: expected font name 'BrushScriptStd', got '"
			     << openType.mCFF.mName.front() << "'" << endl;
			break;
		}

		// Assert: CharStrings INDEX populated and glyph 0 is .notdef (CFF spec invariant).
		if(openType.mCFF.GetCharStringsCount(0) == 0) {
			cout << "CFFIndexSanity: expected non-zero glyph count" << endl;
			break;
		}
		if(openType.mCFF.GetGlyphName(0, 0) != ".notdef") {
			cout << "CFFIndexSanity: expected glyph 0 name '.notdef', got '"
			     << openType.mCFF.GetGlyphName(0, 0) << "'" << endl;
			break;
		}

		// Assert: ReadCharString round-trips a real glyph (proves the end >= start
		// guard didn't accidentally reject the normal end == start + N case).
		CharString* notdef = openType.mCFF.GetGlyphCharString(0, 0);
		if(notdef == NULL) {
			cout << "CFFIndexSanity: GetGlyphCharString returned NULL for .notdef" << endl;
			break;
		}
		EStatusCode readStatus = openType.mCFF.ReadCharString(
			notdef->mStartPosition, notdef->mEndPosition, &buffer);
		if(readStatus != eSuccess) {
			cout << "CFFIndexSanity: ReadCharString failed on a real .notdef glyph" << endl;
			break;
		}
		if(buffer == NULL) {
			cout << "CFFIndexSanity: ReadCharString reported success but left buffer NULL" << endl;
			break;
		}

		ok = true;
	} while(false);

	delete[] buffer;
	return ok;
}

int CFFIndexSanity(int argc, char* argv[]) {
	if(!parsingZeroFirstOffset_returnsFailure()) return 1;
	if(!parsingNonMonotonicOffsets_returnsFailure()) return 1;
	if(!parsingZeroOffSize_returnsFailure()) return 1;
	if(!parsingOutOfRangeOffSize_returnsFailure()) return 1;
	if(!readCharStringWithEndBeforeStart_returnsFailure(argv)) return 1;
	if(!parsingValidCFF_populatesExpectedValues(argv)) return 1;
	return 0;
}
