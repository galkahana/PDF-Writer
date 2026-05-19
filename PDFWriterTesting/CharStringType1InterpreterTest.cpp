/*
   Source File : CharStringType1InterpreterTest.cpp


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


   Tests for CharStringType1Interpreter callothersubr operand-stack
   bounds. Two malicious-charstring cases probe InterpretCallOtherSubr
   (inverted argument-count check drove pop_back() past the empty list)
   and DefaultCallOtherSubr (reverse_iterator walk past rend() leaked
   freed nodes into mPostScriptOperandStack). Two happy-path cases
   prove the bounds checks didn't turn the operators into no-ops by
   exercising the same code paths with valid operands.

   V-059: InterpretDiv divided without guarding the denominator. A
   charstring of `<x> 0 div` triggers integer divide-by-zero (SIGFPE,
   a deterministic process kill), and `<LONG_MIN> -1 div` is
   signed-overflow UB. The fix rejects both before the division.

   Charstring numbers are at most 32-bit (the 0xFF form reads four
   bytes), so the overflow guard's LONG_MIN sentinel is only reachable
   from input where long is 32-bit -- ILP32, or LLP64 such as 64-bit
   Windows: there INT_MIN == LONG_MIN and INT_MIN/-1 overflows, so the
   guard must reject it. Where long is 64-bit (LP64) the same operand
   is -2147483648, INT_MIN/-1 == 2147483648 is representable, no
   overflow occurs, and the division must succeed. The regression case
   asserts the correct outcome for the running platform's long width.

   Charstrings are synthesised in-process: plaintext opcode/operand
   sequences are encoded through the eexec cipher (mirroring
   InputCharStringDecodeStream::DecodeByte) before being handed to the
   interpreter. GetLenIV() is overridden to 0 so no header bytes are
   skipped.
*/
#include "CharStringType1Interpreter.h"
#include "IType1InterpreterImplementation.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include <iostream>
#include <cstring>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Helper that hands the interpreter a synthetic Type1CharString. By
// default IsOtherSubrSupported returns false (the interpreter falls
// through to DefaultCallOtherSubr); a per-test subclass flips it to
// route through CallOtherSubr (which our override leaves the stack
// untouched). GetLenIV() returns 0 so the decode stream consumes no
// header bytes before the first opcode.
class Type1TestHelper : public Type1InterpreterImplementationAdapter {
public:
	virtual unsigned long GetLenIV() { return 0; }
};

class Type1TestHelperWithOtherSubr : public Type1TestHelper {
public:
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex) {
		(void) inOtherSubrsIndex;
		return true;
	}
	virtual EStatusCode CallOtherSubr(const LongList& inOperandList,
	                                  LongList& outPostScriptOperandStack) {
		(void) inOperandList;
		(void) outPostScriptOperandStack;
		return eSuccess;
	}
};

// Mirror InputCharStringDecodeStream::DecodeByte: encrypt a plaintext
// charstring into the ciphertext the decode stream expects to consume.
// The decoder XORs each input byte with (randomizer >> 8) and then
// advances the randomizer using the *encrypted* byte; encryption uses
// the same XOR (it's symmetric) and the same advance, so feeding our
// output back through the decoder reproduces the plaintext exactly.
static void encodeType1CharString(const Byte* inPlain, size_t inLen, Byte* outCipher) {
	const int CONSTANT_1 = 52845;
	const int CONSTANT_2 = 22719;
	const int RANDOMIZER_INIT = 4330;
	const int RANDOMIZER_MODULU_VAL = 65536;

	unsigned short randomizer = RANDOMIZER_INIT;
	for(size_t i = 0; i < inLen; ++i) {
		Byte enc = (Byte)(inPlain[i] ^ (randomizer >> 8));
		outCipher[i] = enc;
		randomizer = (unsigned short)(((enc + randomizer) * CONSTANT_1 + CONSTANT_2) % RANDOMIZER_MODULU_VAL);
	}
}

// Drive the interpreter over a literal plaintext byte sequence. We
// encrypt into a fresh buffer, build a Type1CharString around it, and
// hand it to Intepret(). The buffer is freed after the call.
static EStatusCode interpretPlain(IType1InterpreterImplementation* inHelper,
                                  const char* inPlain, size_t inLen) {
	Byte* cipher = new Byte[inLen];
	encodeType1CharString(reinterpret_cast<const Byte*>(inPlain), inLen, cipher);

	Type1CharString cs;
	cs.Code = cipher;
	cs.CodeLength = (int)inLen;

	CharStringType1Interpreter interp;
	EStatusCode status = interp.Intepret(cs, inHelper);

	delete[] cipher;
	return status;
}

// Pass a raw byte literal to interpretPlain, deriving the length from
// the literal so embedded \x00 bytes don't truncate the string. Use
// only with string literals -- sizeof on a pointer would silently take
// 8 bytes.
#define INTERPRET_PLAIN(helper, bytes) interpretPlain((helper), (bytes), sizeof(bytes) - 1)

// `callothersubr` with argumentsCount=100 on a stack that holds only
// argumentsCount + otherSubrIndex (size 2 after they're popped, then
// size 0 after the count is popped). The interpreter must reject the
// count instead of running pop_back() 100 times on an empty list.
//
// Plaintext bytes:
//   0xEF       push 100   (Type 1 number: 100 = 0xEF - 139)
//   0x8B       push 0     (otherSubrIndex; helper says supported)
//   0x0C 0x10  callothersubr
static bool InterpretCallOtherSubr_ArgumentsCountExceedsStack_ReturnsFailure() {
	// Arrange + Act
	Type1TestHelperWithOtherSubr helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\xEF\x8B\x0C\x10");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType1InterpreterTest: callothersubr with OOB count was accepted" << endl;
		return false;
	}
	return true;
}

// Same plaintext, but the helper does not mark otherSubrIndex 0 as
// supported, so the interpreter routes through DefaultCallOtherSubr.
// The walk loop would step the reverse_iterator 100 positions past
// rend(), leaking sentinel/freed nodes into mPostScriptOperandStack.
//
// Plaintext bytes: same as above.
static bool DefaultCallOtherSubr_ArgumentsCountExceedsStack_ReturnsFailure() {
	// Arrange + Act
	Type1TestHelper helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\xEF\x8B\x0C\x10");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType1InterpreterTest: DefaultCallOtherSubr with OOB count was accepted" << endl;
		return false;
	}
	return true;
}

// Negative argumentsCount must also be rejected. Pre-fix this slipped
// through the buggy `argumentsCount < size()` compare (the long-as-
// size_t conversion turned -1 into a huge unsigned, failing the inverted
// check) and the `for(i=0;i<-1;++i)` loop happened to skip, leaving the
// interpreter in a silently-wrong state. The post-fix `argumentsCount
// < 0` guard pins this case explicitly.
//
// Plaintext bytes:
//   0xFF 0xFF 0xFF 0xFF 0xFF   push -1 (4-byte signed-32 encoding)
//   0x8B                       push 0 (otherSubrIndex; helper says supported)
//   0x0C 0x10                  callothersubr
static bool InterpretCallOtherSubr_NegativeArgumentsCount_ReturnsFailure() {
	// Arrange + Act
	Type1TestHelperWithOtherSubr helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\xFF\xFF\xFF\xFF\xFF\x8B\x0C\x10");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType1InterpreterTest: callothersubr with negative count was accepted" << endl;
		return false;
	}
	return true;
}

// Same plaintext, default route. Pre-fix DefaultCallOtherSubr's
// `for(i=0;i<-1;++i)` skipped without dereferencing past rend(), so
// the function returned success — the negative branch is purely a
// post-fix invariant.
//
// Plaintext bytes: same as above.
static bool DefaultCallOtherSubr_NegativeArgumentsCount_ReturnsFailure() {
	// Arrange + Act
	Type1TestHelper helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\xFF\xFF\xFF\xFF\xFF\x8B\x0C\x10");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType1InterpreterTest: DefaultCallOtherSubr with negative count was accepted" << endl;
		return false;
	}
	return true;
}

// Happy path: 1 real arg, argumentsCount=1, otherSubrIndex=0,
// callothersubr, endchar. The helper marks the index as supported, so
// CallOtherSubr runs (no-op) and the post-call pop loop drains the one
// argument. Confirms the bounds check still admits valid counts.
//
// Plaintext bytes:
//   0x92       push 7     (any arg value; 7 = 0x92 - 139)
//   0x8C       push 1     (argumentsCount)
//   0x8B       push 0     (otherSubrIndex)
//   0x0C 0x10  callothersubr
//   0x0E       endchar
static bool InterpretCallOtherSubr_ValidArgumentsCount_ReturnsSuccess() {
	// Arrange + Act
	Type1TestHelperWithOtherSubr helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\x92\x8C\x8B\x0C\x10\x0E");

	// Assert
	if(status != eSuccess) {
		cout << "CharStringType1InterpreterTest: legitimate callothersubr was rejected" << endl;
		return false;
	}
	return true;
}

// Happy path: same plaintext, helper does not mark the index as
// supported, so DefaultCallOtherSubr runs end-to-end (pushes the one
// arg into mPostScriptOperandStack) and InterpretCallOtherSubr's
// post-call pop loop drains the one argument off the operand stack.
// Confirms DefaultCallOtherSubr's bounds check admits valid counts.
//
// Plaintext bytes: same as above.
static bool DefaultCallOtherSubr_ValidArgumentsCount_ReturnsSuccess() {
	// Arrange + Act
	Type1TestHelper helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\x92\x8C\x8B\x0C\x10\x0E");

	// Assert
	if(status != eSuccess) {
		cout << "CharStringType1InterpreterTest: legitimate DefaultCallOtherSubr was rejected" << endl;
		return false;
	}
	return true;
}

// `5 0 div`: denominator 0. Pre-fix this reached `valueA/valueB` with
// valueB == 0 -> SIGFPE, killing the process. Post-fix InterpretDiv
// rejects it with eFailure.
//
// Plaintext bytes:
//   0x90       push 5  (5 = 0x90 - 139)
//   0x8B       push 0  (0 = 0x8B - 139)
//   0x0C 0x0C  div
static bool InterpretDiv_DivisionByZero_ReturnsFailure() {
	// Arrange + Act
	Type1TestHelper helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\x90\x8B\x0C\x0C");

	// Assert
	if(status == eSuccess) {
		cout << "CharStringType1InterpreterTest: div by zero was accepted" << endl;
		return false;
	}
	return true;
}

// `-2147483648 -1 div`: INT_MIN / -1. Both operands use the 5-byte
// signed-32 number form (0xFF b1 b2 b3 b4). The expected outcome
// depends on the platform's long width (see the file header): where
// long is 32-bit the division overflows and the guard must reject it;
// where long is 64-bit the result 2147483648 is representable and the
// division must succeed.
//
// Plaintext bytes:
//   0xFF 0x80 0x00 0x00 0x00   push -2147483648
//   0xFF 0xFF 0xFF 0xFF 0xFF   push -1
//   0x0C 0x0C                  div
//   0x0E                       endchar (clean terminate on the success path)
static bool InterpretDiv_IntMinOverNegativeOne_HandledPerLongWidth() {
	// Arrange + Act
	Type1TestHelper helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\xFF\x80\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\x0C\x0C\x0E");

	// Assert
	if(sizeof(long) == 4) {
		// INT_MIN == LONG_MIN here: INT_MIN / -1 is signed-overflow UB,
		// the guard must reject it.
		if(status == eSuccess) {
			cout << "CharStringType1InterpreterTest: INT_MIN / -1 accepted where long is 32-bit (overflow UB)" << endl;
			return false;
		}
	} else {
		// long is 64-bit: -2147483648 / -1 == 2147483648 is in range,
		// no overflow, the guard must not fire and the division succeeds.
		if(status != eSuccess) {
			cout << "CharStringType1InterpreterTest: valid INT_MIN / -1 (64-bit long) was rejected" << endl;
			return false;
		}
	}
	return true;
}

// Happy path: `10 2 div` leaves 5 on the stack and endchar terminates
// cleanly. Confirms the denominator guard still admits valid division.
//
// Plaintext bytes:
//   0x95       push 10  (10 = 0x95 - 139)
//   0x8D       push 2   (2 = 0x8D - 139)
//   0x0C 0x0C  div
//   0x0E       endchar
static bool InterpretDiv_ValidDivision_ReturnsSuccess() {
	// Arrange + Act
	Type1TestHelper helper;
	EStatusCode status = INTERPRET_PLAIN(&helper, "\x95\x8D\x0C\x0C\x0E");

	// Assert
	if(status != eSuccess) {
		cout << "CharStringType1InterpreterTest: legitimate `10 2 div` was rejected" << endl;
		return false;
	}
	return true;
}

int CharStringType1InterpreterTest(int argc, char* argv[]) {
	(void) argc;
	(void) argv;
	if(!InterpretCallOtherSubr_ArgumentsCountExceedsStack_ReturnsFailure()) return 1;
	if(!DefaultCallOtherSubr_ArgumentsCountExceedsStack_ReturnsFailure()) return 1;
	if(!InterpretCallOtherSubr_NegativeArgumentsCount_ReturnsFailure()) return 1;
	if(!DefaultCallOtherSubr_NegativeArgumentsCount_ReturnsFailure()) return 1;
	if(!InterpretCallOtherSubr_ValidArgumentsCount_ReturnsSuccess()) return 1;
	if(!DefaultCallOtherSubr_ValidArgumentsCount_ReturnsSuccess()) return 1;
	if(!InterpretDiv_DivisionByZero_ReturnsFailure()) return 1;
	if(!InterpretDiv_IntMinOverNegativeOne_HandledPerLongWidth()) return 1;
	if(!InterpretDiv_ValidDivision_ReturnsSuccess()) return 1;
	return 0;
}
