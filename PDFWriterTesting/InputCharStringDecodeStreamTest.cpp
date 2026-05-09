/*
   Source File : InputCharStringDecodeStreamTest.cpp


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


   Regression test for InputCharStringDecodeStream::Read's increment-on-
   failure bug. Pre-fix the loop did

       status = ReadDecodedByte(inBuffer[bufferIndex]);
       ++bufferIndex;

   so a failed underlying read still advanced bufferIndex and the caller
   saw Read return a count that included an uninitialized slot.
   CharStringType1Interpreter::ProcessCharString reads one byte at a time
   via this Read; pre-fix a wrapped stream that satisfies NotEnded() but
   then returns 0 from its Read would feed the interpreter an
   uninitialized byte and dispatch on it as a CharString opcode.

   Triggering the bug requires the wrapped stream to disagree with
   itself: NotEnded() reporting true while Read() returns 0. Real
   InputByteArrayStream wouldn't: its NotEnded flips to false the
   moment its position reaches its length. So this test uses a small
   IByteReader that lies on purpose, returning zero on Read after a
   configurable cutoff while still claiming NotEnded.
*/
#include "InputCharStringDecodeStream.h"
#include "InputByteArrayStream.h"
#include "IByteReader.h"
#include "IOBasicTypes.h"

#include <cstring>
#include <iostream>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Stream that hands out bytes from a fixed array up to inFailAfter,
// then refuses subsequent Reads while still claiming NotEnded(). This
// is the only way to drive the inner ReadDecodedByte to fail while the
// outer InputCharStringDecodeStream::Read loop's NotEnded check still
// re-enters the body.
class TruncatableReader : public IByteReader {
public:
	TruncatableReader(const Byte* inData, LongBufferSizeType inLen, LongBufferSizeType inFailAfter)
		: mData(inData), mLen(inLen), mPos(0), mFailAfter(inFailAfter) {}

	virtual LongBufferSizeType Read(Byte* outBuf, LongBufferSizeType inCount) {
		if(mPos >= mFailAfter)
			return 0; // refuse — but NotEnded still reports true below
		LongBufferSizeType remaining = mFailAfter - mPos;
		LongBufferSizeType toRead = inCount < remaining ? inCount : remaining;
		memcpy(outBuf, mData + mPos, (size_t)toRead);
		mPos += toRead;
		return toRead;
	}

	virtual bool NotEnded() {
		return mPos < mLen;
	}

private:
	const Byte* mData;
	LongBufferSizeType mLen;
	LongBufferSizeType mPos;
	LongBufferSizeType mFailAfter;
};

// Pre-fix: caller asks for 1 byte, the inner Read returns 0, bufferIndex
// is incremented anyway, Read returns 1 — caller acts on outBuf[0] which
// was never written. Post-fix: Read returns 0 and outBuf[0] is left at
// the caller's sentinel (which itself stays well-defined because
// ReadDecodedByte now zero-initializes the slot too).
static bool Read_TruncatedAtFirstByte_ReturnsZeroAndDoesNotAdvance() {
	Byte fixture[2] = {0x10, 0x20};
	// failAfter = 0: no successful reads ever; NotEnded(true) when claim length=2.
	TruncatableReader src(fixture, /*len=*/2, /*failAfter=*/0);
	InputCharStringDecodeStream cs(&src, /*inLenIV=*/0);

	Byte outBuf[1];
	outBuf[0] = 0xAB; // sentinel — pre-fix V-055 left this slot untouched
	LongBufferSizeType count = cs.Read(outBuf, 1);

	if(count != 0) {
		cout << "InputCharStringDecodeStreamTest: Read returned " << count
		     << " (expected 0 — no underlying byte was actually read)" << endl;
		return false;
	}
	return true;
}

// Multi-byte version of the V-055 mechanism. Caller asks for 4 bytes
// from a stream that yields 2 then refuses. Pre-fix: bufferIndex
// advanced to 3 (the failed third read still bumped it), Read returned
// 3, and slot outBuf[2] was uninitialized — caller would dispatch on
// it as a CharString opcode. Post-fix: Read returns exactly 2; the
// caller's count check stops short of outBuf[2].
static bool Read_TruncatedAfterTwoBytes_ReturnsExactlyTwo() {
	Byte fixture[4] = {0x10, 0x20, 0x30, 0x40};
	TruncatableReader src(fixture, /*len=*/4, /*failAfter=*/2);
	InputCharStringDecodeStream cs(&src, /*inLenIV=*/0);

	Byte outBuf[4];
	memset(outBuf, 0xCD, sizeof(outBuf));
	LongBufferSizeType count = cs.Read(outBuf, 4);

	if(count != 2) {
		cout << "InputCharStringDecodeStreamTest: Read returned " << count
		     << " (expected 2 — only two underlying bytes were readable)" << endl;
		return false;
	}
	return true;
}

// Defense-in-depth check on top of the count fix: ReadDecodedByte itself
// now zero-initializes outByte at entry, so even a buggy caller that
// dispatched on the trailing slot would see deterministic 0 rather than
// stale stack contents or a leftover sentinel.
static bool Read_FailedFirstByte_SlotInitializedToZero() {
	Byte fixture[1] = {0x10};
	TruncatableReader src(fixture, /*len=*/1, /*failAfter=*/0);
	InputCharStringDecodeStream cs(&src, /*inLenIV=*/0);

	Byte outBuf[1];
	outBuf[0] = 0xEF; // sentinel — must not survive the failed read
	LongBufferSizeType count = cs.Read(outBuf, 1);

	if(count != 0) {
		cout << "InputCharStringDecodeStreamTest: count=" << count
		     << " (expected 0)" << endl;
		return false;
	}
	if(outBuf[0] != 0) {
		cout << "InputCharStringDecodeStreamTest: outBuf[0]=" << (int)outBuf[0]
		     << " (expected 0 — ReadDecodedByte must zero outByte on failure)" << endl;
		return false;
	}
	return true;
}

// Encode three plaintext bytes with the same scheme InputCharStringDecodeStream
// undoes — an in-test mirror of the eexec cipher used elsewhere in the
// PFB-loading tests. Lets the round-trip test below assert exact decoded
// values without needing to read magic numbers out of the production code.
static void encodePlaintext(const Byte* inPlain, size_t inLen, Byte* outEncoded) {
	const unsigned int kConstant1 = 52845;
	const unsigned int kConstant2 = 22719;
	unsigned short randomizer = 4330; // RANDOMIZER_INIT
	for(size_t i = 0; i < inLen; ++i) {
		Byte cipher = (Byte)(inPlain[i] ^ (randomizer >> 8));
		outEncoded[i] = cipher;
		randomizer = (unsigned short)((cipher + randomizer) * kConstant1 + kConstant2);
	}
}

// Happy-path: encode then decode three known plaintext bytes through
// the real InputCharStringDecodeStream pipeline; confirm the decoded
// bytes match the original plaintext exactly. Catches a regression that
// would, e.g., always exit before writing the first byte.
static bool Read_FullyAvailableStream_RoundTripsPlaintext() {
	const Byte plaintext[3] = {0x42, 0x9C, 0x01};
	Byte encoded[3] = {0};
	encodePlaintext(plaintext, 3, encoded);

	InputByteArrayStream src(encoded, sizeof(encoded));
	InputCharStringDecodeStream cs(&src, /*inLenIV=*/0);

	Byte outBuf[3];
	memset(outBuf, 0, sizeof(outBuf));
	LongBufferSizeType count = cs.Read(outBuf, 3);

	if(count != 3) {
		cout << "InputCharStringDecodeStreamTest: count=" << count
		     << " (expected 3)" << endl;
		return false;
	}
	if(memcmp(outBuf, plaintext, 3) != 0) {
		cout << "InputCharStringDecodeStreamTest: round-trip mismatch (got 0x"
		     << hex << (int)outBuf[0] << " 0x" << (int)outBuf[1] << " 0x"
		     << (int)outBuf[2] << dec << ", expected plaintext 0x42 0x9C 0x01)" << endl;
		return false;
	}
	return true;
}

// Larger buffer than the stream can fill. Honest underlying stream's
// NotEnded flips to false at end, so the loop exits naturally and Read
// returns the actual byte count. This is the path that worked even
// pre-fix; included to pin the boundary.
static bool Read_BufferLargerThanStream_ReturnsActualByteCount() {
	Byte fixture[2] = {0x10, 0x20};
	InputByteArrayStream src(fixture, sizeof(fixture));
	InputCharStringDecodeStream cs(&src, /*inLenIV=*/0);

	Byte outBuf[5];
	memset(outBuf, 0xAA, sizeof(outBuf));
	LongBufferSizeType count = cs.Read(outBuf, 5);

	if(count != 2) {
		cout << "InputCharStringDecodeStreamTest: count=" << count
		     << " (expected 2)" << endl;
		return false;
	}
	// Trailing slots untouched.
	for(size_t i = 2; i < 5; ++i) {
		if(outBuf[i] != 0xAA) {
			cout << "InputCharStringDecodeStreamTest: outBuf[" << i << "]=" << (int)outBuf[i]
			     << " (expected sentinel 0xAA — trailing slot should be untouched)" << endl;
			return false;
		}
	}
	return true;
}

int InputCharStringDecodeStreamTest(int argc, char* argv[]) {
	if(!Read_TruncatedAtFirstByte_ReturnsZeroAndDoesNotAdvance()) return 1;
	if(!Read_TruncatedAfterTwoBytes_ReturnsExactlyTwo()) return 1;
	if(!Read_FailedFirstByte_SlotInitializedToZero()) return 1;
	if(!Read_FullyAvailableStream_RoundTripsPlaintext()) return 1;
	if(!Read_BufferLargerThanStream_ReturnsActualByteCount()) return 1;
	return 0;
}
