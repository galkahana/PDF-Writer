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
*/
#include "InputPFBDecodeStream.h"
#include "EStatusCode.h"

#include <iostream>
#include <new>
#include <cstring>

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

int InputPFBDecodeStreamTest(int argc, char* argv[]) {
	(void)argc;
	if(!Read_FreshlyConstructedWithoutAssign_ReturnsZeroAndDisclosesNothing(argv)) return 1;
	return 0;
}
