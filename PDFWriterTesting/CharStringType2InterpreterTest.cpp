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

int CharStringType2InterpreterTest(int argc, char* argv[]) {
	(void) argc;
	(void) argv;
	if(!InterpretIndex_OperandOutOfRange_ReturnsFailure()) return 1;
	if(!InterpretPut_SlotOutOfRange_ReturnsFailure()) return 1;
	if(!InterpretIndex_ValidOperand_ReturnsSuccess()) return 1;
	if(!InterpretPut_ValidSlotRoundTrip_ReturnsSuccess()) return 1;
	return 0;
}
