/*
   Source File : OpenTypePrimitiveReaderTest.cpp


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


   Regression test for OpenTypePrimitiveReader's stale-outValue contract.
   Pre-fix every Read* short-circuited on prior failure (or wrote
   uninitialized stack memory on read failure) without writing outValue,
   so callers that ignored the return code observed whatever the local was
   initialized to. The most damaging consumer was OpenTypeFileInput's
   composite-glyph dependency loop (V-017): on a truncated `glyf` entry the
   `flags` local kept its previous value, and if MORE_COMPONENTS (bit 32)
   was set, the do/while spun forever growing `mComponentGlyphs` until OOM.

   The fix initializes outValue at function entry across the reader, so
   every Read* failure path now leaves outValue == 0. The simulated-loop
   test below proves this terminates V-017's heap-growth-until-OOM DoS;
   the per-primitive tests pin the new contract one type at a time.
*/
#include "OpenTypePrimitiveReader.h"
#include "InputByteArrayStream.h"
#include "InputFile.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Empty stream: every Read* fails on first byte.
static InputByteArrayStream& emptyStream() {
	static Byte sEmpty[1] = {0};
	static InputByteArrayStream sStream(sEmpty, 0);
	sStream.SetPosition(0);
	return sStream;
}

static bool ReadBYTE_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	unsigned char value = 0xAB;
	EStatusCode status = reader.ReadBYTE(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadBYTE on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadBYTE left outValue=" << (int)value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadCHAR_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	char value = (char)0x55;
	EStatusCode status = reader.ReadCHAR(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadCHAR on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadCHAR left outValue=" << (int)value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadUSHORT_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	unsigned short value = 0xCDEF;
	EStatusCode status = reader.ReadUSHORT(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadUSHORT on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadUSHORT left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// Truncated mid-read: USHORT needs 2 bytes; only 1 is available. Pre-fix the
// second ReadBYTE short-circuited and ReadUSHORT returned without writing
// outValue at all.
static bool ReadUSHORT_TruncatedMidRead_OutValueZero() {
	Byte oneByte[1] = {0xAA};
	InputByteArrayStream stream(oneByte, 1);
	OpenTypePrimitiveReader reader(&stream);
	unsigned short value = 0xCDEF;
	EStatusCode status = reader.ReadUSHORT(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadUSHORT mid-truncation returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadUSHORT mid-truncation left outValue="
		     << value << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadSHORT_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	short value = 0x1234;
	EStatusCode status = reader.ReadSHORT(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadSHORT on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadSHORT left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadULONG_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	unsigned long value = 0xDEADBEEFUL;
	EStatusCode status = reader.ReadULONG(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadULONG on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadULONG left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadLONG_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	long value = 0x12345678L;
	EStatusCode status = reader.ReadLONG(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadLONG on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadLONG left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

static bool ReadFixed_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	double value = 3.14;
	EStatusCode status = reader.ReadFixed(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadFixed on empty stream returned success" << endl;
		return false;
	}
	if(value != 0.0) {
		cout << "OpenTypePrimitiveReaderTest: ReadFixed left outValue=" << value
		     << " (expected 0.0)" << endl;
		return false;
	}
	return true;
}

static bool ReadLongDateTime_PastEOF_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());
	long long value = 0x1122334455667788LL;
	EStatusCode status = reader.ReadLongDateTime(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadLongDateTime on empty stream returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadLongDateTime left outValue=" << value
		     << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// Pre-fix: once a read failed and mInternalState went to eFailure, every
// subsequent Read* short-circuited without writing outValue. Caller's
// previous-iteration value would persist. Post-fix: outValue stays 0.
static bool ReadUSHORT_AfterPriorFailure_OutValueZero() {
	OpenTypePrimitiveReader reader(&emptyStream());

	// First read fails and sticks the reader in eFailure.
	unsigned char throwaway = 0;
	if(reader.ReadBYTE(throwaway) == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: priming ReadBYTE unexpectedly succeeded" << endl;
		return false;
	}
	if(reader.GetInternalState() != eFailure) {
		cout << "OpenTypePrimitiveReaderTest: reader did not enter eFailure after priming" << endl;
		return false;
	}

	// Second read on an already-failed reader. Pre-fix outValue retained 0xCDEF.
	unsigned short value = 0xCDEF;
	EStatusCode status = reader.ReadUSHORT(value);
	if(status == eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: ReadUSHORT on failed reader returned success" << endl;
		return false;
	}
	if(value != 0) {
		cout << "OpenTypePrimitiveReaderTest: ReadUSHORT on failed reader left outValue="
		     << value << " (expected 0)" << endl;
		return false;
	}
	return true;
}

// V-017 mechanism reproduction. Walks the same control flow as
// OpenTypeFileInput::ReadGlyfForDependencies's composite-glyph loop,
// driving it from a primitive reader over a 1-byte stream so the very
// first ReadUSHORT(flags) inside the loop body fails. Pre-fix `flags`
// kept its prior 0xFFFF value (MORE_COMPONENTS bit set), and the loop
// spun until killed; post-fix flags becomes 0 on the failed read and the
// MORE_COMPONENTS bit is clear, terminating the loop.
static bool GlyfDependencyLoop_TruncatedFlags_TerminatesNotInfinite() {
	Byte oneByte[1] = {0xFF};
	InputByteArrayStream stream(oneByte, 1);
	OpenTypePrimitiveReader reader(&stream);

	// Mirror ReadGlyfForDependencies' locals: `flags` carries over between
	// iterations and is what V-017's infinite loop depended on.
	unsigned short flags = 0xFFFF; // MORE_COMPONENTS bit set
	unsigned short glyphIndex = 0;
	const long kSafetyLimit = 100000L;
	long iterations = 0;

	do {
		++iterations;
		if(iterations > kSafetyLimit) {
			cout << "OpenTypePrimitiveReaderTest: composite-glyph loop hit safety limit ("
			     << kSafetyLimit << " iterations) — V-017 still reachable" << endl;
			return false;
		}
		reader.ReadUSHORT(flags);
		reader.ReadUSHORT(glyphIndex);
	} while((flags & 32) != 0);

	// Post-fix: exactly one iteration. The first ReadUSHORT failure cleared
	// `flags` to 0, the MORE_COMPONENTS bit (0x20) is clear, loop exits.
	if(iterations != 1) {
		cout << "OpenTypePrimitiveReaderTest: composite-glyph loop ran " << iterations
		     << " times (expected exactly 1 post-fix)" << endl;
		return false;
	}
	if(flags != 0) {
		cout << "OpenTypePrimitiveReaderTest: flags=" << flags << " after failed read (expected 0)" << endl;
		return false;
	}
	return true;
}

// Happy path: reading a real font's SFNT version still produces the
// expected exact bytes. Catches a regression that would, e.g., always
// reset outValue to 0 after a successful read.
static bool ReadULONG_ArialSfntVersion_Returns0x00010000(char* argv[]) {
	InputFile ttfFile;
	if(ttfFile.OpenFile(BuildRelativeInputPath(argv, "fonts/arial.ttf")) != eSuccess) {
		cout << "OpenTypePrimitiveReaderTest: failed to open arial.ttf" << endl;
		return false;
	}

	bool ok = false;
	do {
		OpenTypePrimitiveReader reader(ttfFile.GetInputStream());
		unsigned long sfntVersion = 0;
		if(reader.ReadULONG(sfntVersion) != eSuccess) {
			cout << "OpenTypePrimitiveReaderTest: ReadULONG(sfntVersion) failed on arial.ttf" << endl;
			break;
		}
		// TrueType outline fonts begin with sfnt version 0x00010000.
		if(sfntVersion != 0x00010000UL) {
			cout << "OpenTypePrimitiveReaderTest: arial.ttf sfntVersion=0x" << hex << sfntVersion
			     << dec << " (expected 0x00010000)" << endl;
			break;
		}
		ok = true;
	} while(false);

	return ok;
}

int OpenTypePrimitiveReaderTest(int argc, char* argv[]) {
	if(!ReadBYTE_PastEOF_OutValueZero()) return 1;
	if(!ReadCHAR_PastEOF_OutValueZero()) return 1;
	if(!ReadUSHORT_PastEOF_OutValueZero()) return 1;
	if(!ReadUSHORT_TruncatedMidRead_OutValueZero()) return 1;
	if(!ReadSHORT_PastEOF_OutValueZero()) return 1;
	if(!ReadULONG_PastEOF_OutValueZero()) return 1;
	if(!ReadLONG_PastEOF_OutValueZero()) return 1;
	if(!ReadFixed_PastEOF_OutValueZero()) return 1;
	if(!ReadLongDateTime_PastEOF_OutValueZero()) return 1;
	if(!ReadUSHORT_AfterPriorFailure_OutValueZero()) return 1;
	if(!GlyfDependencyLoop_TruncatedFlags_TerminatesNotInfinite()) return 1;
	if(!ReadULONG_ArialSfntVersion_Returns0x00010000(argv)) return 1;
	return 0;
}
