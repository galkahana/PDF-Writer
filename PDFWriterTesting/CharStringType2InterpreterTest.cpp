/*
   Source File : CharStringType2InterpreterTest.cpp


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


   Tests for CharStringType2Interpreter operand-stack and transient-array
   bounds. Two malicious-charstring cases probe InterpretIndex (walked
   past rend() and dereferenced the sentinel) and InterpretPut (wrote
   into a never-sized mStorage at attacker-supplied index). Two
   happy-path cases prove the fix didn't turn the operators into no-ops,
   exercising the same code paths with valid operands.

   V-029: InterpretCallSubr / InterpretCallGSubr read .IntegerValue from
   the CharStringOperand union without checking IsInteger. When the
   subroutine-index operand was pushed as a real (the 0xFF 16.16
   fixed-point opcode), .IntegerValue reinterprets the IEEE-754 double's
   bits as a long, handing the implementation an attacker-controlled
   junk index. The fix coerces a real operand with (long)RealValue, the
   same idiom every other operator in this file already uses. The
   regression cases push a real 5.0 as the subr index and assert the
   implementation receives exactly 5 (pre-fix it received the
   reinterpreted bit pattern, a large non-5 value).

   Charstrings are synthesised in-process via a minimal
   IType2InterpreterImplementation harness, no font fixtures required.
*/
#include "CharStringType2Interpreter.h"
#include "IType2InterpreterImplementation.h"
#include "CharStringDefinitions.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include <iostream>
#include <cstring>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Type2InterpreterImplementationAdapter's defaults return eSuccess for
// every operator callback; we only override ReadCharString to hand back
// our synthetic byte buffer. Intepret() takes ownership of the returned
// buffer and frees it.
class FixedBytesHelper : public Type2InterpreterImplementationAdapter {
public:
	FixedBytesHelper(const char* inBytes, size_t inLen) : mBytes(inBytes), mLen(inLen) {}
	virtual EStatusCode ReadCharString(LongFilePositionType inStart,
	                                   LongFilePositionType inEnd,
	                                   Byte** outCharString) {
		(void) inStart;
		(void) inEnd;
		Byte* buf = new Byte[mLen];
		memcpy(buf, mBytes, mLen);
		*outCharString = buf;
		return eSuccess;
	}
private:
	const char* mBytes;
	size_t mLen;
};

// Drive the interpreter over a literal byte sequence. The CharString
// span sets the read length; ReadCharString materialises the bytes.
static EStatusCode interpretBytes(const char* inBytes, size_t inLen) {
	FixedBytesHelper helper(inBytes, inLen);
	CharString cs;
	cs.mStartPosition = 0;
	cs.mEndPosition = (LongFilePositionType)inLen;
	CharStringType2Interpreter interp;
	return interp.Intepret(cs, &helper);
}

// Pass a raw byte literal to interpretBytes, deriving the length from
// the literal so embedded \x00 bytes don't truncate the string. Use only
// with string literals -- sizeof on a pointer would silently take 8 bytes.
#define INTERPRET(bytes) interpretBytes((bytes), sizeof(bytes) - 1)

// `index N` requires the operand stack to hold at least N+1 entries
// after the index value itself is popped. The interpreter must reject N
// values that exceed the available depth instead of walking past
// rend() and dereferencing the list sentinel.
//
// Bytes: 0xEF (push 100), 0x0C 0x1D (index)
// After pop the stack has size 0 but N is 100, so the walk would step
// 100 nodes past rend().
static bool InterpretIndex_OperandOutOfRange_ReturnsFailure() {
	// Arrange + Act
	EStatusCode status = INTERPRET("\xEF\x0C\x1D");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType2InterpreterTest: index with OOB operand was accepted" << endl;
		return false;
	}
	return true;
}

// `put` writes valueA into mStorage[slot]. The transient array has 32
// entries; any slot >= 32 must be rejected before the indexed write.
//
// Bytes: 0x90 (push 5, the value), 0xEF (push 100, the slot),
//        0x0C 0x14 (put)
static bool InterpretPut_SlotOutOfRange_ReturnsFailure() {
	// Arrange + Act
	EStatusCode status = INTERPRET("\x90\xEF\x0C\x14");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType2InterpreterTest: put with OOB slot was accepted" << endl;
		return false;
	}
	return true;
}

// Happy path: `index 1` on a 2-deep stack picks the second-from-top
// operand. Confirms that the bounds check still allows valid in-range
// walks and that the loop body now actually advances/decrements
// correctly (the original loop never decremented index, so even
// legitimate non-zero indices walked past rend()).
//
// Bytes: 0x90 (push 5), 0x95 (push 10), 0x8C (push 1, the index arg),
//        0x0C 0x1D (index), 0x0E (endchar)
static bool InterpretIndex_ValidOperand_ReturnsSuccess() {
	// Arrange + Act
	EStatusCode status = INTERPRET("\x90\x95\x8C\x0C\x1D\x0E");

	// Assert
	if(status != eSuccess) {
		cout << "CharStringType2InterpreterTest: legitimate `index 1` was rejected" << endl;
		return false;
	}
	return true;
}

// Happy path: `put 5 0` then `get 0` round-trips a value through slot 0
// of the transient array. Confirms the fix sized mStorage on entry to
// Intepret() so a legitimate put (and the matching get) succeed.
//
// Bytes: 0x90 (push 5, the value), 0x8B (push 0, the slot),
//        0x0C 0x14 (put), 0x8B (push 0), 0x0C 0x15 (get), 0x0E (endchar)
static bool InterpretPut_ValidSlotRoundTrip_ReturnsSuccess() {
	// Arrange + Act
	EStatusCode status = INTERPRET("\x90\x8B\x0C\x14\x8B\x0C\x15\x0E");

	// Assert
	if(status != eSuccess) {
		cout << "CharStringType2InterpreterTest: legitimate put/get round-trip was rejected" << endl;
		return false;
	}
	return true;
}

// Records the index the interpreter passes to GetLocalSubr /
// GetGlobalSubr so the test can assert the operand was coerced to the
// integer the charstring actually encoded, not the bit-reinterpretation
// of a real. Overrides ReadCharString to hand back the synthetic
// program: Intepret pulls the bytes through ReadCharString, not from
// the CharString span, so without this the interpreter would never see
// the program and never reach the subr lookup. Returning NULL from the
// lookup aborts interpretation right after the recording, which is all
// the assertion needs.
class SubrIndexRecorder : public Type2InterpreterImplementationAdapter {
public:
	SubrIndexRecorder() : mBytes(NULL), mLen(0),
	                      mLocalIndex(0), mGlobalIndex(0),
	                      mLocalCalled(false), mGlobalCalled(false) {}
	void SetProgram(const char* inBytes, size_t inLen) {
		mBytes = inBytes;
		mLen = inLen;
	}
	virtual EStatusCode ReadCharString(LongFilePositionType inStart,
	                                   LongFilePositionType inEnd,
	                                   Byte** outCharString) {
		(void) inStart;
		(void) inEnd;
		Byte* buf = new Byte[mLen];
		memcpy(buf, mBytes, mLen);
		*outCharString = buf;
		return eSuccess;
	}
	virtual CharString* GetLocalSubr(long inSubrIndex) {
		mLocalIndex = inSubrIndex;
		mLocalCalled = true;
		return NULL;
	}
	virtual CharString* GetGlobalSubr(long inSubrIndex) {
		mGlobalIndex = inSubrIndex;
		mGlobalCalled = true;
		return NULL;
	}
	const char* mBytes;
	size_t mLen;
	long mLocalIndex;
	long mGlobalIndex;
	bool mLocalCalled;
	bool mGlobalCalled;
};

static EStatusCode interpretBytesWith(SubrIndexRecorder& inHelper,
                                      const char* inBytes, size_t inLen) {
	inHelper.SetProgram(inBytes, inLen);
	CharString cs;
	cs.mStartPosition = 0;
	cs.mEndPosition = (LongFilePositionType)inLen;
	CharStringType2Interpreter interp;
	return interp.Intepret(cs, &inHelper);
}

#define INTERPRET_WITH(helper, bytes) interpretBytesWith((helper), (bytes), sizeof(bytes) - 1)

// `0xFF 00 05 00 00` pushes the 16.16 real 5.0. `callsubr` (opcode 10)
// then uses it as the local-subr index. Pre-fix the union's
// .IntegerValue read reinterpreted 5.0's IEEE-754 bits as a long,
// handing GetLocalSubr a huge junk value. Post-fix the real is coerced
// with (long)RealValue, so GetLocalSubr must receive exactly 5.
static bool InterpretCallSubr_RealIndexOperand_CoercesToInteger() {
	// Arrange
	SubrIndexRecorder helper;

	// Act
	INTERPRET_WITH(helper, "\xFF\x00\x05\x00\x00\x0A");

	// Assert
	if(!helper.mLocalCalled) {
		cout << "CharStringType2InterpreterTest: GetLocalSubr was never called" << endl;
		return false;
	}
	if(helper.mLocalIndex != 5) {
		cout << "CharStringType2InterpreterTest: real subr index not coerced; GetLocalSubr got "
		     << helper.mLocalIndex << " expected 5" << endl;
		return false;
	}
	return true;
}

// Same as above but `callgsubr` (opcode 29) routes through
// GetGlobalSubr.
static bool InterpretCallGSubr_RealIndexOperand_CoercesToInteger() {
	// Arrange
	SubrIndexRecorder helper;

	// Act
	INTERPRET_WITH(helper, "\xFF\x00\x05\x00\x00\x1D");

	// Assert
	if(!helper.mGlobalCalled) {
		cout << "CharStringType2InterpreterTest: GetGlobalSubr was never called" << endl;
		return false;
	}
	if(helper.mGlobalIndex != 5) {
		cout << "CharStringType2InterpreterTest: real subr index not coerced; GetGlobalSubr got "
		     << helper.mGlobalIndex << " expected 5" << endl;
		return false;
	}
	return true;
}

// Happy path: an integer subr-index operand (byte 0x90 = 5 + 139) must
// still reach GetLocalSubr unchanged. Confirms the coercion didn't
// disturb the common integer path.
static bool InterpretCallSubr_IntegerIndexOperand_PassesThrough() {
	// Arrange
	SubrIndexRecorder helper;

	// Act
	INTERPRET_WITH(helper, "\x90\x0A");

	// Assert
	if(!helper.mLocalCalled) {
		cout << "CharStringType2InterpreterTest: GetLocalSubr was never called" << endl;
		return false;
	}
	if(helper.mLocalIndex != 5) {
		cout << "CharStringType2InterpreterTest: integer subr index altered; GetLocalSubr got "
		     << helper.mLocalIndex << " expected 5" << endl;
		return false;
	}
	return true;
}

int CharStringType2InterpreterTest(int argc, char* argv[]) {
	(void) argc;
	(void) argv;
	if(!InterpretIndex_OperandOutOfRange_ReturnsFailure()) return 1;
	if(!InterpretPut_SlotOutOfRange_ReturnsFailure()) return 1;
	if(!InterpretIndex_ValidOperand_ReturnsSuccess()) return 1;
	if(!InterpretPut_ValidSlotRoundTrip_ReturnsSuccess()) return 1;
	if(!InterpretCallSubr_RealIndexOperand_CoercesToInteger()) return 1;
	if(!InterpretCallGSubr_RealIndexOperand_CoercesToInteger()) return 1;
	if(!InterpretCallSubr_IntegerIndexOperand_PassesThrough()) return 1;
	return 0;
}
