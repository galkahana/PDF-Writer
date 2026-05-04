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


   Regression test for the CFF INDEX offset-table sanity checks added in
   cluster C5 (V-033/034/036/037). Each malformed CFF below would previously
   reach an unsigned subtraction on garbage offsets and feed the result to
   `new Byte[N]`, a bad_alloc / OOB primitive depending on the values. With
   the validation in ReadIndexHeader (and the defensive guard in
   ReadCharString) in place, every malformed prefix is rejected with
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

// Compose a CFF prefix: 4-byte header followed by a Name INDEX that the
// caller crafted. The other INDEXes are never reached because parsing
// fails at the Name INDEX.
static string BuildCFFPrefix(const string& inNameIndex) {
	string cff;
	// Header: major=1, minor=0, hdrSize=4, absOffSize=1. The fourth byte's
	// value is irrelevant for the rest of parsing — ReadCFFFile only uses
	// hdrSize to skip to the Name INDEX, which is already at offset 4.
	cff.push_back((char)0x01);
	cff.push_back((char)0x00);
	cff.push_back((char)0x04);
	cff.push_back((char)0x01);
	cff += inNameIndex;
	return cff;
}

static EStatusCode parseSyntheticCFF(const string& inNameIndex) {
	string cff = BuildCFFPrefix(inNameIndex);
	InputByteArrayStream stream((Byte*)&cff[0], (LongFilePositionType)cff.size());
	CFFFileInput cffInput;
	return cffInput.ReadCFFFile(&stream);
}

// V-036: offsets[0] < 1 used to underflow `dataStartPosition + offsets[0] - 1`
// in ReadSubrsFromIndex (and produce wild charstring start positions).
static bool parsingZeroFirstOffset_returnsFailure() {
	// Arrange: count=1, offSize=1, offsets=[0x00, 0x01]
	string nameIndex;
	nameIndex.push_back((char)0x00); nameIndex.push_back((char)0x01); // count=1
	nameIndex.push_back((char)0x01);                                  // offSize=1
	nameIndex.push_back((char)0x00);                                  // offsets[0]=0
	nameIndex.push_back((char)0x01);                                  // offsets[1]=1
	nameIndex.push_back((char)'X');                                   // dummy data byte

	// Act
	EStatusCode status = parseSyntheticCFF(nameIndex);

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: zero first offset was accepted (V-036 regressed)" << endl;
		return false;
	}
	return true;
}

// V-036: non-monotonic offsets used to make `offsets[i+1] - offsets[i]` wrap
// to ~ULONG_MAX in unsigned arithmetic, then drive `new Byte[N]`.
static bool parsingNonMonotonicOffsets_returnsFailure() {
	// Arrange: count=1, offSize=1, offsets=[0x05, 0x01] (1 < 5 -> non-monotonic)
	string nameIndex;
	nameIndex.push_back((char)0x00); nameIndex.push_back((char)0x01); // count=1
	nameIndex.push_back((char)0x01);                                  // offSize=1
	nameIndex.push_back((char)0x05);                                  // offsets[0]=5
	nameIndex.push_back((char)0x01);                                  // offsets[1]=1 (< prev)

	// Act
	EStatusCode status = parseSyntheticCFF(nameIndex);

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: non-monotonic offsets were accepted (V-036 regressed)" << endl;
		return false;
	}
	return true;
}

// V-034: offSize=0 leaves ReadOffset's switch with no matching case,
// returning eFailure but with offset values uninitialized in the buffer.
static bool parsingZeroOffSize_returnsFailure() {
	// Arrange: count=1, offSize=0
	string nameIndex;
	nameIndex.push_back((char)0x00); nameIndex.push_back((char)0x01); // count=1
	nameIndex.push_back((char)0x00);                                  // offSize=0 (invalid)

	// Act
	EStatusCode status = parseSyntheticCFF(nameIndex);

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: offSize=0 was accepted (V-034 regressed)" << endl;
		return false;
	}
	return true;
}

// V-034: same with an out-of-range high value.
static bool parsingOutOfRangeOffSize_returnsFailure() {
	// Arrange: count=1, offSize=5
	string nameIndex;
	nameIndex.push_back((char)0x00); nameIndex.push_back((char)0x01); // count=1
	nameIndex.push_back((char)0x05);                                  // offSize=5 (invalid)

	// Act
	EStatusCode status = parseSyntheticCFF(nameIndex);

	// Assert
	if(status == eSuccess) {
		cout << "CFFIndexSanity: offSize=5 was accepted (V-034 regressed)" << endl;
		return false;
	}
	return true;
}

// Parses BrushScriptStd.otf and leaves the file open through the caller's
// scope so the CFF reader's stream pointer stays valid for follow-up
// ReadCharString calls. Returns eSuccess and populates outOpenType+outFile,
// otherwise returns eFailure (file is closed in the failure path).
static EStatusCode openBrushScriptStd(char* argv[], InputFile& outFile, OpenTypeFileInput& outOpenType) {
	if(outFile.OpenFile(BuildRelativeInputPath(argv, "fonts/BrushScriptStd.otf")) != eSuccess)
		return eFailure;
	EStatusCode status = outOpenType.ReadOpenTypeFile(outFile.GetInputStream(), 0);
	if(status != eSuccess)
		outFile.CloseFile();
	return status;
}

// V-037: ReadCharString with end < start used to underflow the unsigned
// subtraction fed to `new Byte[N]`. Reachable directly via the public
// IType2InterpreterImplementation::ReadCharString override.
static bool readCharStringWithEndBeforeStart_returnsFailure(char* argv[]) {
	// Arrange: parse a valid font so mPrimitivesReader has a stream.
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(openBrushScriptStd(argv, otfFile, openType) != eSuccess) {
		cout << "CFFIndexSanity: BrushScriptStd.otf parse failed; cannot test V-037" << endl;
		return false;
	}

	// Act: pick legitimate-looking offsets but with end strictly less than start.
	Byte* charString = NULL;
	EStatusCode status = openType.mCFF.ReadCharString(/*start*/ 1000, /*end*/ 500, &charString);
	otfFile.CloseFile();

	// Assert
	if(status == eSuccess) {
		delete[] charString;
		cout << "CFFIndexSanity: ReadCharString accepted end < start (V-037 regressed)" << endl;
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

	// Assert: Name INDEX produced exactly one font with the expected name.
	bool ok = true;
	if(openType.mCFF.mFontsCount != 1) {
		cout << "CFFIndexSanity: expected 1 font, got " << openType.mCFF.mFontsCount << endl;
		ok = false;
	}
	if(ok && openType.mCFF.mName.size() != 1) {
		cout << "CFFIndexSanity: expected 1 name entry, got " << openType.mCFF.mName.size() << endl;
		ok = false;
	}
	if(ok && openType.mCFF.mName.front() != "BrushScriptStd") {
		cout << "CFFIndexSanity: expected font name 'BrushScriptStd', got '"
		     << openType.mCFF.mName.front() << "'" << endl;
		ok = false;
	}

	// Assert: CharStrings INDEX populated and glyph 0 is .notdef (CFF spec invariant).
	unsigned short glyphCount = openType.mCFF.GetCharStringsCount(0);
	if(ok && glyphCount == 0) {
		cout << "CFFIndexSanity: expected non-zero glyph count" << endl;
		ok = false;
	}
	if(ok && openType.mCFF.GetGlyphName(0, 0) != ".notdef") {
		cout << "CFFIndexSanity: expected glyph 0 name '.notdef', got '"
		     << openType.mCFF.GetGlyphName(0, 0) << "'" << endl;
		ok = false;
	}

	// Assert: ReadCharString round-trips a real glyph (proves our V-037 guard
	// didn't accidentally reject end == start + N for any positive N).
	if(ok) {
		CharString* notdef = openType.mCFF.GetGlyphCharString(0, 0);
		if(notdef == NULL) {
			cout << "CFFIndexSanity: GetGlyphCharString returned NULL for .notdef" << endl;
			ok = false;
		} else {
			Byte* buffer = NULL;
			EStatusCode readStatus = openType.mCFF.ReadCharString(
				notdef->mStartPosition, notdef->mEndPosition, &buffer);
			if(readStatus != eSuccess) {
				cout << "CFFIndexSanity: ReadCharString failed on a real .notdef glyph" << endl;
				ok = false;
			} else if(buffer == NULL) {
				cout << "CFFIndexSanity: ReadCharString reported success but left buffer NULL" << endl;
				ok = false;
			}
			delete[] buffer;
		}
	}

	otfFile.CloseFile();
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
