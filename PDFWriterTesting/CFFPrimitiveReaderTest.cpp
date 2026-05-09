/*
   Source File : CFFPrimitiveReaderTest.cpp


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


   Regression test for CFFPrimitiveReader's stale-outValue contract.
   Pre-fix every ReadByte / ReadCard16 / Read{3,4}ByteUnsigned (and the
   signed/wrapper variants) returned eFailure without writing outValue
   when the underlying stream was exhausted or the reader was already in
   sticky-failure state. Callers in CFFFileInput::ReadEncoding /
   ReadFDSelect deliberately discarded the per-call status (V-050) and
   drove heap allocations and array indices off whatever the local
   happened to hold — partial garbage on a truncated CFF.

   The fix initializes outValue at function entry across the named
   primitives, so failure paths now consistently leave outValue == 0.
   The discarded-status pattern in ReadEncoding / ReadFDSelect is no
   longer dangerous: the operands those callers feed into allocations
   and into mFDArray / mFDSelect indexing are deterministically zero
   when the reader has failed.

   The 3- and 4-byte readers and their signed variants are private; this
   test reaches the unsigned ones through ReadOffset(SetOffSize(3|4)),
   which is the only public route into them and the route every caller
   in CFFFileInput uses.
*/
#include "CFFPrimitiveReader.h"
#include "InputByteArrayStream.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Empty stream: every Read* fails on the first byte.
static InputByteArrayStream& emptyStream() {
	static Byte sEmpty[1] = {0};
	static InputByteArrayStream sStream(sEmpty, 0);
	sStream.SetPosition(0);
	return sStream;
}

static bool ReadByte_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	Byte value = 0xAB;
	EStatusCode status = reader.ReadByte(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadByte on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadByte left outValue=" << (int)value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadCard8_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	Byte value = 0x99;
	EStatusCode status = reader.ReadCard8(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadCard8 on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadCard8 left outValue=" << (int)value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadCard16_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	unsigned short value = 0xCDEF;
	EStatusCode status = reader.ReadCard16(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadCard16 on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadCard16 left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// Truncated mid-read: Card16 needs 2 bytes; only 1 is available. Pre-fix the
// second ReadByte short-circuited and ReadCard16 returned without writing
// outValue at all.
static bool ReadCard16_TruncatedMidRead_OutValueZero() {
	Byte oneByte[1] = {0xAA};
	InputByteArrayStream stream(oneByte, 1);
	CFFPrimitiveReader reader(&stream);
	unsigned short value = 0xCDEF;
	EStatusCode status = reader.ReadCard16(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadCard16 mid-truncation returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadCard16 mid-truncation left outValue="
		     << value << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadSID_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	unsigned short value = 0xBEEF;
	EStatusCode status = reader.ReadSID(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadSID on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadSID left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadOffSize_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	Byte value = 0x77;
	EStatusCode status = reader.ReadOffSize(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffSize on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadOffSize left outValue=" << (int)value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// ReadOffset with offSize=1 routes through ReadCard8.
static bool ReadOffset_OffSize1_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	reader.SetOffSize(1);
	unsigned long value = 0xCAFEBABEUL;
	EStatusCode status = reader.ReadOffset(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=1) on empty returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=1) left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// ReadOffset with offSize=2 routes through ReadCard16.
static bool ReadOffset_OffSize2_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	reader.SetOffSize(2);
	unsigned long value = 0xCAFEBABEUL;
	EStatusCode status = reader.ReadOffset(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=2) on empty returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=2) left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// ReadOffset with offSize=3 routes through the private Read3ByteUnsigned.
static bool ReadOffset_OffSize3_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	reader.SetOffSize(3);
	unsigned long value = 0xCAFEBABEUL;
	EStatusCode status = reader.ReadOffset(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=3) on empty returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=3) left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// ReadOffset with offSize=4 routes through the private Read4ByteUnsigned.
static bool ReadOffset_OffSize4_PastEOF_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());
	reader.SetOffSize(4);
	unsigned long value = 0xCAFEBABEUL;
	EStatusCode status = reader.ReadOffset(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=4) on empty returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=4) left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// ReadOffset's switch has no `default` branch — when mCurrentOffsize is
// outside {1..4} it returns the entry-time `status = eFailure` without
// touching outValue. Pre-fix outValue stayed at the caller's value
// (uninitialized for all the in-tree callers); post-fix it's 0.
static bool ReadOffset_InvalidOffSize_OutValueZero() {
	Byte plenty[8] = {0};
	InputByteArrayStream stream(plenty, sizeof(plenty));
	CFFPrimitiveReader reader(&stream);
	reader.SetOffSize(0); // outside {1..4}
	unsigned long value = 0x12345678UL;
	EStatusCode status = reader.ReadOffset(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffset with invalid offSize returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=0) left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// Pre-fix: once a read failed and mInternalState went to eFailure, every
// subsequent Read* short-circuited without writing outValue. Caller's
// previous value would persist. Post-fix: outValue stays 0.
static bool ReadCard16_AfterPriorFailure_OutValueZero() {
	CFFPrimitiveReader reader(&emptyStream());

	// Prime the reader into eFailure.
	Byte throwaway = 0;
	if(reader.ReadByte(throwaway) == eSuccess) {
		cout << "CFFPrimitiveReaderTest: priming ReadByte unexpectedly succeeded" << endl;
		return false;
	}
	if(reader.GetInternalState() != eFailure) {
		cout << "CFFPrimitiveReaderTest: reader did not enter eFailure after priming" << endl;
		return false;
	}

	// Second read on an already-failed reader. Pre-fix outValue retained 0xCDEF.
	unsigned short value = 0xCDEF;
	EStatusCode status = reader.ReadCard16(value);
	if(status == eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadCard16 on failed reader returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "CFFPrimitiveReaderTest: ReadCard16 on failed reader left outValue="
		     << value << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// V-050 mechanism reproduction. Mirrors the inner loop of
// CFFFileInput::ReadFDSelect format 3, which deliberately discards each
// ReadCard8 / ReadCard16 status and only consults GetInternalState() at
// the end of every iteration. Pre-fix `fdIndex` and `nextRangeGlyphIndex`
// kept their previous-iteration values across truncation, driving
// `mFDArray + fdIndex` indexing and the inner glyph-range walk with
// stale data. Post-fix both stay 0 once the reader fails — empty range,
// zero offset, no heap corruption.
static bool FDSelectInnerLoop_TruncatedStream_OperandsZeroAfterFailure() {
	// 1-byte stream: first ReadCard8 succeeds, ReadCard16 then fails and
	// the reader sticks in eFailure. Subsequent loop iterations' reads
	// are sticky-failure short-circuits.
	Byte oneByte[1] = {0x07};
	InputByteArrayStream stream(oneByte, 1);
	CFFPrimitiveReader reader(&stream);

	Byte fdIndex = 0xAB;
	unsigned short nextRangeGlyphIndex = 0xCDEF;

	// Iteration 1: Card8 succeeds (consumes the one byte), Card16 fails.
	reader.ReadCard8(fdIndex);
	reader.ReadCard16(nextRangeGlyphIndex);
	if(reader.GetInternalState() != eFailure) {
		cout << "CFFPrimitiveReaderTest: reader did not enter eFailure after truncated Card16" << endl;
		return false;
	}
	if(fdIndex != 0x07) {
		cout << "CFFPrimitiveReaderTest: iter 1 fdIndex=" << (int)fdIndex
		     << " (expected 0x07 from successful Card8)" << endl;
		return false;
	}
	if(nextRangeGlyphIndex != 0) {
		cout << "CFFPrimitiveReaderTest: iter 1 nextRangeGlyphIndex=" << nextRangeGlyphIndex
		     << " (expected 0 — failed Card16 must not leak stale value)" << endl;
		return false;
	}

	// Iteration 2: both reads are sticky-failure short-circuits. Pre-fix,
	// fdIndex and nextRangeGlyphIndex carried over the iteration-1 values.
	// Post-fix both reset to 0 inside the failed Read*.
	reader.ReadCard8(fdIndex);
	reader.ReadCard16(nextRangeGlyphIndex);
	if(fdIndex != 0) {
		cout << "CFFPrimitiveReaderTest: iter 2 fdIndex=" << (int)fdIndex
		     << " (expected 0 after sticky failure)" << endl;
		return false;
	}
	if(nextRangeGlyphIndex != 0) {
		cout << "CFFPrimitiveReaderTest: iter 2 nextRangeGlyphIndex=" << nextRangeGlyphIndex
		     << " (expected 0 after sticky failure)" << endl;
		return false;
	}

	return true;
}

// Happy path for the offSize=4 route through Read4ByteUnsigned. Catches
// a regression that would, e.g., always reset outValue to 0 after a
// successful read.
static bool ReadOffset_OffSize4_KnownBytes_ReturnsExpectedBigEndianValue() {
	Byte bytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
	InputByteArrayStream stream(bytes, 4);
	CFFPrimitiveReader reader(&stream);
	reader.SetOffSize(4);

	unsigned long value = 0;
	if(reader.ReadOffset(value) != eSuccess) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=4) on 4-byte stream failed" << endl;
		return false;
	}
	if(value != 0xDEADBEEFUL) {
		cout << "CFFPrimitiveReaderTest: ReadOffset(offSize=4) returned 0x" << hex << value
		     << dec << " (expected 0xDEADBEEF)" << endl;
		return false;
	}
	return true;
}

int CFFPrimitiveReaderTest(int argc, char* argv[]) {
	if(!ReadByte_PastEOF_OutValueZero()) return 1;
	if(!ReadCard8_PastEOF_OutValueZero()) return 1;
	if(!ReadCard16_PastEOF_OutValueZero()) return 1;
	if(!ReadCard16_TruncatedMidRead_OutValueZero()) return 1;
	if(!ReadSID_PastEOF_OutValueZero()) return 1;
	if(!ReadOffSize_PastEOF_OutValueZero()) return 1;
	if(!ReadOffset_OffSize1_PastEOF_OutValueZero()) return 1;
	if(!ReadOffset_OffSize2_PastEOF_OutValueZero()) return 1;
	if(!ReadOffset_OffSize3_PastEOF_OutValueZero()) return 1;
	if(!ReadOffset_OffSize4_PastEOF_OutValueZero()) return 1;
	if(!ReadOffset_InvalidOffSize_OutValueZero()) return 1;
	if(!ReadCard16_AfterPriorFailure_OutValueZero()) return 1;
	if(!FDSelectInnerLoop_TruncatedStream_OperandsZeroAfterFailure()) return 1;
	if(!ReadOffset_OffSize4_KnownBytes_ReturnsExpectedBigEndianValue()) return 1;
	return 0;
}
