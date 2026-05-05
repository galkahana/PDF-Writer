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


   Regression test for CFF DICT operand-list bounds in
   GetSingleIntegerValueFromDict and ReadPrivateDict. Both used to call
   front()/back() on the operand list without checking it was non-empty
   or that the operand was actually an integer, so a malformed CFF that
   emitted an operator with no operands (or with a real where the spec
   requires an integer) reached UB / type-punned union reads that flowed
   into seek offsets and ReadDict's read amount.

   The malformed streams are synthesised in-process so no binary fixtures
   are required. The happy-path test parses BrushScriptStd.otf and asserts
   exact private-dict values so we can tell the new validation didn't
   accidentally turn the parser into a no-op.
*/
#include "InputByteArrayStream.h"
#include "InputFile.h"
#include "OpenTypeFileInput.h"
#include "CFFFileInput.h"
#include "DictOperand.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// CFF spec keys (operator bytes) used to drive the bug paths from a Top DICT.
static const Byte scOpCharStrings = 0x11;   // CharStrings (key 17)
static const Byte scOpPrivate     = 0x12;   // Private (key 18)

// Header: major=1, minor=0, hdrSize=4, absOffSize=1.
static const char scCFFHeader[] = "\x01\x00\x04\x01";

// Empty INDEXes for String INDEX and Global Subrs INDEX, which we don't
// exercise but ReadCFFFile insists on parsing before reaching ReadCharStrings
// and ReadPrivateDicts.
static const char scEmptyIndex[] = "\x00\x00";

// Build a complete CFF buffer ending at the Global Subrs INDEX. The Top DICT
// payload (the part between the operator-encoded Top DICT INDEX header and the
// String INDEX) is supplied by the caller so each test crafts the malformed
// region it cares about. Top DICT length is capped at 254 so it fits a single
// 1-byte offset entry.
static string buildSyntheticCFF(const char* inTopDictBytes, size_t inTopDictLen) {
	string cff;
	cff.append(scCFFHeader, sizeof(scCFFHeader) - 1);

	// Name INDEX: count=1, offSize=1, offsets=[1, 2], data="A"
	cff.append("\x00\x01\x01\x01\x02\x41", 6);

	// Top DICT INDEX: count=1, offSize=1, offsets=[1, 1+len], data=<top dict>
	cff.append("\x00\x01\x01\x01", 4);
	cff.push_back((char)(1 + (Byte)inTopDictLen));
	cff.append(inTopDictBytes, inTopDictLen);

	// String INDEX (empty), Global Subrs INDEX (empty)
	cff.append(scEmptyIndex, sizeof(scEmptyIndex) - 1);
	cff.append(scEmptyIndex, sizeof(scEmptyIndex) - 1);

	return cff;
}

static EStatusCode parseSyntheticCFF(const char* inTopDictBytes, size_t inTopDictLen, CFFFileInput& outCFF) {
	string cff = buildSyntheticCFF(inTopDictBytes, inTopDictLen);
	InputByteArrayStream stream((Byte*)&cff[0], (LongFilePositionType)cff.size());
	return outCFF.ReadCFFFile(&stream);
}

// Pass a raw byte literal to parseSyntheticCFF, deriving the length from
// the literal so embedded \x00 bytes don't truncate. Use only with string
// literals — sizeof on a pointer would silently take 8 bytes.
#define PARSE_TOP_DICT(cff, bytes) parseSyntheticCFF((bytes), sizeof(bytes) - 1, (cff))

// /CharStrings (key 17) with no operand: pre-fix, GetCharStringsPosition
// did `front().IntegerValue` on an empty list and seeded SetOffset/ReadCard16
// with garbage. Post-fix, the empty list maps to the supplied default (0),
// which makes ReadCharStrings a no-op and lets ReadCFFFile complete cleanly.
static bool GetSingleIntegerValueFromDict_EmptyOperandList_FallsBackToDefault() {
	// Arrange: top dict = [<op CharStrings>] — single operator, zero operands.
	CFFFileInput cff;
	EStatusCode status = PARSE_TOP_DICT(cff, "\x11");

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest: ReadCFFFile failed for empty-operand CharStrings key" << endl;
		return false;
	}
	if(cff.GetCharStringsCount(0) != 0) {
		cout << "CFFFileInputTest: expected 0 charstrings, got " << cff.GetCharStringsCount(0) << endl;
		return false;
	}
	return true;
}

// /CharStrings (key 17) with a real operand: pre-fix, IntegerValue was read
// from the union after RealValue had been written, yielding a platform-
// dependent bit-pattern that became a wild SetOffset target. Post-fix the
// non-integer operand falls back to the default and parsing continues.
static bool GetSingleIntegerValueFromDict_RealOperand_FallsBackToDefault() {
	// Arrange: top dict = [<real 0.5> <op CharStrings>].
	// Real BCD: 0x1E header, nibbles {0, '.'(0xa), 5, end(0xf)} = bytes 0x0A 0x5F.
	CFFFileInput cff;
	EStatusCode status = PARSE_TOP_DICT(cff, "\x1E\x0A\x5F\x11");

	// Assert
	if(status != eSuccess) {
		cout << "CFFFileInputTest: ReadCFFFile failed for real-operand CharStrings key" << endl;
		return false;
	}
	if(cff.GetCharStringsCount(0) != 0) {
		cout << "CFFFileInputTest: expected 0 charstrings (real-operand fallback), got "
		     << cff.GetCharStringsCount(0) << endl;
		return false;
	}
	return true;
}

// /Private (key 18) with no operands: pre-fix, both front() and back() on
// the empty list were UB; the resulting garbage was passed to SetOffset
// and ReadDict. Post-fix, the empty list is rejected with eFailure.
static bool ReadPrivateDict_EmptyOperandList_ReturnsFailure() {
	// Arrange: top dict = [<op Private>]
	CFFFileInput cff;
	EStatusCode status = PARSE_TOP_DICT(cff, "\x12");

	// Assert
	if(status == eSuccess) {
		cout << "CFFFileInputTest: empty-operand Private was accepted" << endl;
		return false;
	}
	return true;
}

// /Private with a single integer operand: pre-fix, front()==back() so size
// and offset became the same value; in particular, with operand 0 ReadDict
// silently parses zero bytes and ReadCFFFile returns success — exercising
// the "silently wrong" pre-fix branch (no crash, no wrong-offset failure).
// Post-fix, size() < 2 is rejected with eFailure.
static bool ReadPrivateDict_SingleOperand_ReturnsFailure() {
	// Arrange: top dict = [<int 0> <op Private>]. Encoding for int 0 is 0x8B
	// (139 - 139). Single operand for what spec requires to be (size, offset).
	CFFFileInput cff;
	EStatusCode status = PARSE_TOP_DICT(cff, "\x8B\x12");

	// Assert
	if(status == eSuccess) {
		cout << "CFFFileInputTest: single-operand Private was accepted" << endl;
		return false;
	}
	return true;
}

// /Private with a real where an integer is required: pre-fix, IntegerValue
// reads were taken from the union over RealValue, yielding garbage offsets
// fed to SetOffset / ReadDict. Post-fix, non-integer operands are rejected.
static bool ReadPrivateDict_NonIntegerOperand_ReturnsFailure() {
	// Arrange: top dict = [<real 0.5> <int 0> <op Private>]. Two operands,
	// but the first (size) is a real — exactly the union-misuse case.
	CFFFileInput cff;
	EStatusCode status = PARSE_TOP_DICT(cff, "\x1E\x0A\x5F\x8B\x12");

	// Assert
	if(status == eSuccess) {
		cout << "CFFFileInputTest: real-operand Private was accepted" << endl;
		return false;
	}
	return true;
}

// Happy path: prove the new validation didn't break parsing of a real CFF
// font. Asserts exact expected values rather than just "didn't crash" /
// "size > 0", so a regression that quietly turns the parser into a no-op
// would be caught.
static bool ReadCFFFile_BrushScriptStd_PopulatesPrivateDict(char* argv[]) {
	// Arrange
	InputFile otfFile;
	OpenTypeFileInput openType;
	if(otfFile.OpenFile(BuildRelativeInputPath(argv, "fonts/BrushScriptStd.otf")) != eSuccess) {
		cout << "CFFFileInputTest: failed to open BrushScriptStd.otf" << endl;
		return false;
	}
	if(openType.ReadOpenTypeFile(otfFile.GetInputStream(), 0) != eSuccess) {
		cout << "CFFFileInputTest: positive path failed - real CFF rejected" << endl;
		return false;
	}

	bool ok = false;

	do {
		// Assert: exactly one Private DICT, span is non-empty (start < end).
		if(openType.mCFF.mFontsCount != 1) {
			cout << "CFFFileInputTest: expected 1 font, got " << openType.mCFF.mFontsCount << endl;
			break;
		}
		if(openType.mCFF.mPrivateDicts == NULL) {
			cout << "CFFFileInputTest: mPrivateDicts is NULL" << endl;
			break;
		}
		const PrivateDictInfo& priv = openType.mCFF.mPrivateDicts[0];
		// Exact byte positions for BrushScriptStd's Private DICT (offset=17676, size=28).
		if(priv.mPrivateDictStart != 17676) {
			cout << "CFFFileInputTest: expected Private DICT start 17676, got "
			     << priv.mPrivateDictStart << endl;
			break;
		}
		if(priv.mPrivateDictEnd != 17704) {
			cout << "CFFFileInputTest: expected Private DICT end 17704, got "
			     << priv.mPrivateDictEnd << endl;
			break;
		}
		// Sanity-check that ReadDict actually populated the inner dict.
		if(priv.mPrivateDict.empty()) {
			cout << "CFFFileInputTest: inner Private DICT is empty" << endl;
			break;
		}

		ok = true;
	} while(false);

	return ok;
}

int CFFFileInputTest(int argc, char* argv[]) {
	if(!GetSingleIntegerValueFromDict_EmptyOperandList_FallsBackToDefault()) return 1;
	if(!GetSingleIntegerValueFromDict_RealOperand_FallsBackToDefault()) return 1;
	if(!ReadPrivateDict_EmptyOperandList_ReturnsFailure()) return 1;
	if(!ReadPrivateDict_SingleOperand_ReturnsFailure()) return 1;
	if(!ReadPrivateDict_NonIntegerOperand_ReturnsFailure()) return 1;
	if(!ReadCFFFile_BrushScriptStd_PopulatesPrivateDict(argv)) return 1;
	return 0;
}
