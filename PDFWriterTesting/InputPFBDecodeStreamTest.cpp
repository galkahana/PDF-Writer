/*
   Source File : InputPFBDecodeStreamTest.cpp


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


   Regression test for the InputPFBDecodeStream constructor leaving
   mInSegmentReadIndex / mSegmentSize / mCurrentType / mHasTokenBuffer /
   mTokenBuffer / mRandomizer / mFoundEOF uninitialized. Only Assign() (via
   ResetReadStatus) sets most of them, so a freshly constructed stream used
   without a successful Assign reads indeterminate values. In particular
   Read() tests mHasTokenBuffer first and, if it is non-zero, copies the
   indeterminate mTokenBuffer into the caller's buffer and returns 1 — an
   uninitialized-memory disclosure plus a wrong byte count.

   The object is constructed into storage pre-filled with 0xFF so a missing
   constructor initializer is deterministically observable (a plain stack
   object's uninitialized members are often incidentally zero on a given
   build, masking the bug). Post-fix the constructor zeroes every member, so
   Read() on the un-Assigned stream returns 0 and writes nothing.

   Also covers the PFB segment-length assembly: a declared length whose high
   byte is >= 0x80 must yield the correct large unsigned value. The old
   `byte4 << 24` performed the shift in `int`, which is signed-overflow UB
   when byte4 >= 0x80 and (on two's-complement) produced a negative
   mSegmentSize -- the loop bound -- so the segment was silently treated as
   zero-length and skipped.
*/
#include "InputPFBDecodeStream.h"
#include "InputByteArrayStream.h"
#include "EStatusCode.h"

#include <iostream>
#include <new>
#include <cstring>
#include <string>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

static bool Read_FreshlyConstructedWithoutAssign_ReturnsZeroAndDisclosesNothing(char* /*argv*/[]) {
	// Arrange: construct over 0xFF-poisoned storage so any member the
	// constructor fails to initialize stays 0xFF rather than incidental zero.
	void* storage = ::operator new(sizeof(InputPFBDecodeStream));
	memset(storage, 0xFF, sizeof(InputPFBDecodeStream));
	InputPFBDecodeStream* stream = new (storage) InputPFBDecodeStream();

	const Byte sentinel = 0xAB;
	Byte out[4];
	memset(out, sentinel, sizeof(out));

	// Act: Read with no successful Assign. Pre-fix, the indeterminate
	// mHasTokenBuffer (0xFF -> true) makes Read emit the indeterminate
	// mTokenBuffer and return 1.
	LongBufferSizeType got = stream->Read(out, sizeof(out));

	// Assert
	bool ok = true;
	if(got != 0) {
		cout << "InputPFBDecodeStreamTest: fresh Read returned " << got << " bytes, expected 0" << endl;
		ok = false;
	}
	if(out[0] != sentinel) {
		cout << "InputPFBDecodeStreamTest: fresh Read overwrote caller buffer with 0x"
		     << hex << (unsigned int)out[0] << dec << " (uninitialized-memory disclosure)" << endl;
		ok = false;
	}
	if(stream->NotEnded()) {
		cout << "InputPFBDecodeStreamTest: fresh NotEnded() is true, expected false" << endl;
		ok = false;
	}
	if(stream->GetInternalState() != eFailure) {
		cout << "InputPFBDecodeStreamTest: fresh GetInternalState() is not eFailure" << endl;
		ok = false;
	}

	stream->~InputPFBDecodeStream();
	::operator delete(storage);
	return ok;
}

// One row exercises StoreSegmentLength (observed through Read): a type-1
// ASCII PFB segment whose declared length high byte is mHighByte, carrying a
// short "abc" payload. The declared length is always far larger than the
// payload, so Read drains the 3 payload bytes and returns 3. Pre-fix, a
// mHighByte >= 0x80 made mSegmentSize negative (signed-shift UB), the inner
// read loop never ran, and Read returned 0.
struct StoreSegmentLengthCase {
	const char* mLabel;
	Byte mHighByte;
};

static const StoreSegmentLengthCase scStoreSegmentLengthCases[] = {
	{ "HighByteZero_ReadsPayload",        0x00 },
	{ "HighByteSignBit_ReadsPayload",     0x80 },
	{ "HighByteAllOnes_ReadsPayload",     0xFF },
};

static bool RunStoreSegmentLengthCases() {
	const char payload[3] = { 'a', 'b', 'c' };
	const size_t caseCount = sizeof(scStoreSegmentLengthCases) / sizeof(scStoreSegmentLengthCases[0]);
	bool ok = true;

	for(size_t i = 0; i < caseCount; ++i) {
		const StoreSegmentLengthCase& testCase = scStoreSegmentLengthCases[i];

		// Arrange: type-1 ASCII segment header (0x80 0x01) + 4-byte LE
		// declared length 0xFF 0x00 0x00 <highByte> (always >> 3) + 3 payload
		// bytes, then the source stream simply ends.
		std::string pfb;
		pfb.push_back((char)0x80);
		pfb.push_back((char)0x01);
		pfb.push_back((char)0xFF);
		pfb.push_back((char)0x00);
		pfb.push_back((char)0x00);
		pfb.push_back((char)testCase.mHighByte);
		pfb.append(payload, sizeof(payload));

		InputByteArrayStream* source = new InputByteArrayStream((Byte*)&pfb[0], (LongFilePositionType)pfb.size());
		InputPFBDecodeStream stream;
		EStatusCode assigned = stream.Assign(source);

		const Byte sentinel = 0xAB;
		Byte out[8];
		memset(out, sentinel, sizeof(out));

		// Act
		LongBufferSizeType got = stream.Read(out, sizeof(out));

		// Assert: segment length decoded as a large positive value, so the
		// whole payload is delivered.
		if(assigned != eSuccess) {
			cout << "InputPFBDecodeStreamTest[" << testCase.mLabel
			     << "]: Assign returned non-eSuccess" << endl;
			ok = false;
		}
		if(got != 3) {
			cout << "InputPFBDecodeStreamTest[" << testCase.mLabel
			     << "]: Read returned " << got << ", expected 3 (segment skipped: signed-shift UB?)" << endl;
			ok = false;
		}
		if(!(out[0] == 'a' && out[1] == 'b' && out[2] == 'c')) {
			cout << "InputPFBDecodeStreamTest[" << testCase.mLabel
			     << "]: payload bytes not 'abc'" << endl;
			ok = false;
		}
	}
	return ok;
}

int InputPFBDecodeStreamTest(int argc, char* argv[]) {
	(void)argc;
	if(!Read_FreshlyConstructedWithoutAssign_ReturnsZeroAndDisclosesNothing(argv)) return 1;
	if(!RunStoreSegmentLengthCases()) return 1;
	return 0;
}
