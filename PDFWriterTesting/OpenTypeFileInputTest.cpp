/*
   Source File : OpenTypeFileInputTest.cpp


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


   Regression test for ReadHMtx's numberOfHMetrics-zero guard. The existing
   guard bounded numberOfHMetrics > numGlyphs, but missed the case
   numberOfHMetrics == 0 with numGlyphs > 0. The second loop in ReadHMtx
   then dereferences mHMtx[NumberOfHMetrics-1], where unsigned short 0 - 1
   promotes to int -1: a one-before-buffer heap read whose contents flow
   into the produced PDF's /Widths array.

   The negative case patches a real font in memory (arial.ttf) and feeds
   the modified buffer through ReadOpenTypeFile. The happy path uses the
   same buffer-driven path so a regression that turned the new validation
   into a no-op-rejecting parser would also be caught.
*/
#include "InputByteArrayStream.h"
#include "InputFile.h"
#include "OpenTypeFileInput.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>
#include <cstring>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Read entire file into outBytes. Returns false on open / size / read failure.
static bool readFileBytes(const string& inPath, string& outBytes) {
	InputFile file;
	if(file.OpenFile(inPath) != eSuccess)
		return false;
	LongFilePositionType size = file.GetFileSize();
	if(size <= 0)
		return false;
	outBytes.resize((size_t)size);
	LongBufferSizeType wanted = (LongBufferSizeType)size;
	LongBufferSizeType got = file.GetInputStream()->Read((Byte*)&outBytes[0], wanted);
	return got == wanted;
}

// Locate the byte offset of an SFNT table by 4-byte tag. Returns (size_t)-1 on
// missing table or malformed directory. Assumes the buffer starts at the SFNT
// header (no resource fork wrapper).
static size_t findTableOffset(const string& inFont, const char* inTag) {
	if(inFont.size() < 12)
		return (size_t)-1;
	const Byte* p = (const Byte*)inFont.data();
	unsigned short numTables = (unsigned short)((p[4] << 8) | p[5]);
	if(inFont.size() < 12u + (size_t)numTables * 16u)
		return (size_t)-1;
	for(unsigned short i = 0; i < numTables; ++i) {
		size_t entry = 12u + (size_t)i * 16u;
		if(p[entry+0] == (Byte)inTag[0] && p[entry+1] == (Byte)inTag[1]
		&& p[entry+2] == (Byte)inTag[2] && p[entry+3] == (Byte)inTag[3]) {
			return ((size_t)p[entry+8] << 24) | ((size_t)p[entry+9] << 16)
			     | ((size_t)p[entry+10] << 8) | (size_t)p[entry+11];
		}
	}
	return (size_t)-1;
}

static unsigned short readU16BE(const string& inBuf, size_t inPos) {
	return (unsigned short)(((Byte)inBuf[inPos] << 8) | (Byte)inBuf[inPos+1]);
}

// Length field of an SFNT table directory entry (entry+12, big-endian u32).
// Returns (size_t)-1 on missing table or malformed directory.
static size_t findTableLength(const string& inFont, const char* inTag) {
	if(inFont.size() < 12)
		return (size_t)-1;
	const Byte* p = (const Byte*)inFont.data();
	unsigned short numTables = (unsigned short)((p[4] << 8) | p[5]);
	if(inFont.size() < 12u + (size_t)numTables * 16u)
		return (size_t)-1;
	for(unsigned short i = 0; i < numTables; ++i) {
		size_t entry = 12u + (size_t)i * 16u;
		if(p[entry+0] == (Byte)inTag[0] && p[entry+1] == (Byte)inTag[1]
		&& p[entry+2] == (Byte)inTag[2] && p[entry+3] == (Byte)inTag[3]) {
			return ((size_t)p[entry+12] << 24) | ((size_t)p[entry+13] << 16)
			     | ((size_t)p[entry+14] << 8) | (size_t)p[entry+15];
		}
	}
	return (size_t)-1;
}

// Pre-fix: ReadOpenTypeSFNT's ttcf branch did mHeaderOffset += offsetTable then
// recursed unconditionally. An offset-table entry of 0 leaves mHeaderOffset
// unchanged, so the recursion re-reads the same ttcf header forever until the
// stack is exhausted. The buffer below is a minimal ttcf header whose single
// font offset is 0. Post-fix the zero entry is rejected before recursing, so
// the call returns eFailure instead of crashing. Reaching the assertion at all
// proves the unbounded recursion is gone.
static bool ReadOpenTypeSFNT_TtcfZeroOffsetTable_ReturnsFailure(char* argv[]) {
	(void)argv;
	// Arrange — 'ttcf', version 1.0, numFonts 1, offsetTable[0] = 0
	// (non-const to match InputByteArrayStream's mutable-pointer constructor)
	Byte ttc[16] = {
		0x74,0x74,0x63,0x66,  0x00,0x01,0x00,0x00,
		0x00,0x00,0x00,0x01,  0x00,0x00,0x00,0x00
	};

	// Act
	InputByteArrayStream stream(ttc, (LongFilePositionType)sizeof(ttc));
	OpenTypeFileInput openType;
	EStatusCode status = openType.ReadOpenTypeFile(&stream, 0);

	// Assert
	if(status == eSuccess) {
		cout << "OpenTypeFileInputTest: zero ttcf offset table was accepted" << endl;
		return false;
	}
	return true;
}

// Pre-fix: ReadName allocated new char[Length] and Read Length bytes from
// nameOffset + stringOffset + entryOffset with no check that the range stays
// inside the name table, so a crafted Length pulled bytes out of an adjacent
// table into the entry's String (and from there into the produced PDF).
// Patching name entry 0's Length to 0xFFFF makes its range escape the table.
// Post-fix that entry is clamped to empty (Length 0) and parsing still
// succeeds (a malformed sub-entry must not reject the whole font).
static bool ReadName_StringRangeEscapesNameTable_ClampsEntry(char* argv[]) {
	// Arrange
	string font;
	if(!readFileBytes(BuildRelativeInputPath(argv, "fonts/arial.ttf"), font)) {
		cout << "OpenTypeFileInputTest: failed to read arial.ttf" << endl;
		return false;
	}
	size_t nameOffset = findTableOffset(font, "name");
	if(nameOffset == (size_t)-1 || nameOffset + 14 + 2 > font.size()) {
		cout << "OpenTypeFileInputTest: arial.ttf name table missing or truncated" << endl;
		return false;
	}
	// Header: format(2) count(2) stringOffset(2); entry 0 Length is the 5th
	// USHORT of the first 6-USHORT record (offset 6 + 8).
	font[nameOffset + 14] = (char)0xFF;
	font[nameOffset + 15] = (char)0xFF;

	// Act
	InputByteArrayStream stream((Byte*)&font[0], (LongFilePositionType)font.size());
	OpenTypeFileInput* openType = new OpenTypeFileInput();
	EStatusCode status = openType->ReadOpenTypeFile(&stream, 0);

	// Assert
	bool ok = false;
	do {
		if(status != eSuccess) {
			cout << "OpenTypeFileInputTest: a single out-of-range name entry rejected the whole font" << endl;
			break;
		}
		if(openType->mName.mNameEntries[0].Length != 0) {
			cout << "OpenTypeFileInputTest: out-of-range name entry 0 not clamped (Length "
			     << openType->mName.mNameEntries[0].Length << ")" << endl;
			break;
		}
		ok = true;
	} while(false);

	delete openType;
	return ok;
}

// Pre-fix: ReadGlyfForDependencies used loca offsets as glyf seek positions
// with no monotonicity / in-table check. The last loca entry is not itself a
// seek base, so zeroing it leaves a pristine pre-fix parse succeeding while
// making loca[NumGlyphs] < loca[NumGlyphs-1]. Post-fix the non-monotonic loca
// is rejected.
static bool ReadGlyfForDependencies_LocaNotMonotonic_ReturnsFailure(char* argv[]) {
	// Arrange
	string font;
	if(!readFileBytes(BuildRelativeInputPath(argv, "fonts/arial.ttf"), font)) {
		cout << "OpenTypeFileInputTest: failed to read arial.ttf" << endl;
		return false;
	}
	size_t headOffset = findTableOffset(font, "head");
	size_t maxpOffset = findTableOffset(font, "maxp");
	size_t locaOffset = findTableOffset(font, "loca");
	size_t locaLength = findTableLength(font, "loca");
	if(headOffset == (size_t)-1 || maxpOffset == (size_t)-1
	|| locaOffset == (size_t)-1 || locaLength == (size_t)-1
	|| headOffset + 52 > font.size() || maxpOffset + 6 > font.size()) {
		cout << "OpenTypeFileInputTest: arial.ttf head/maxp/loca tables missing or truncated" << endl;
		return false;
	}
	unsigned short indexToLocFormat = readU16BE(font, headOffset + 50);
	unsigned short numGlyphs = readU16BE(font, maxpOffset + 4);
	// Last loca entry is at index numGlyphs; width 2 (short) or 4 (long).
	size_t lastEntry = (indexToLocFormat == 0)
		? locaOffset + (size_t)numGlyphs * 2
		: locaOffset + (size_t)numGlyphs * 4;
	size_t entryWidth = (indexToLocFormat == 0) ? 2u : 4u;
	if(lastEntry + entryWidth > locaOffset + locaLength || lastEntry + entryWidth > font.size()) {
		cout << "OpenTypeFileInputTest: computed loca last-entry position out of range" << endl;
		return false;
	}
	for(size_t b = 0; b < entryWidth; ++b)
		font[lastEntry + b] = 0;

	// Act
	InputByteArrayStream stream((Byte*)&font[0], (LongFilePositionType)font.size());
	OpenTypeFileInput openType;
	EStatusCode status = openType.ReadOpenTypeFile(&stream, 0);

	// Assert
	if(status == eSuccess) {
		cout << "OpenTypeFileInputTest: non-monotonic loca was accepted" << endl;
		return false;
	}
	return true;
}

// Pre-fix: the existing > NumGlyphs guard accepted numberOfHMetrics == 0,
// then the second loop indexed mHMtx[NumberOfHMetrics-1]. (unsigned short)0
// minus int 1 promotes to int -1, so the access reads one HMtxTableEntry
// before the heap allocation. ReadOpenTypeFile returned eSuccess and the
// out-of-bounds AdvanceWidth landed in /Widths. Post-fix the reader rejects
// this case before allocating mHMtx.
static bool ReadHMtx_NumberOfHMetricsZero_ReturnsFailure(char* argv[]) {
	// Arrange
	string font;
	if(!readFileBytes(BuildRelativeInputPath(argv, "fonts/arial.ttf"), font)) {
		cout << "OpenTypeFileInputTest: failed to read arial.ttf" << endl;
		return false;
	}
	size_t hheaOffset = findTableOffset(font, "hhea");
	if(hheaOffset == (size_t)-1 || hheaOffset + 36 > font.size()) {
		cout << "OpenTypeFileInputTest: arial.ttf hhea table missing or truncated" << endl;
		return false;
	}
	// numberOfHMetrics is the last USHORT in hhea (offset 34, big-endian).
	font[hheaOffset + 34] = 0;
	font[hheaOffset + 35] = 0;

	// Act
	InputByteArrayStream stream((Byte*)&font[0], (LongFilePositionType)font.size());
	OpenTypeFileInput openType;
	EStatusCode status = openType.ReadOpenTypeFile(&stream, 0);

	// Assert
	if(status == eSuccess) {
		cout << "OpenTypeFileInputTest: numberOfHMetrics=0 was accepted" << endl;
		return false;
	}
	return true;
}

// Happy path: same buffer-driven path used by the negative case, unmodified
// arial.ttf parses successfully and the relevant tables get populated.
// Asserts exact values rather than bounds so a future regression that
// quietly turns the new guard into a no-op-rejecting branch would surface.
static bool ReadOpenTypeFile_ArialTtf_PopulatesHheaMaxp(char* argv[]) {
	// Arrange
	string font;
	if(!readFileBytes(BuildRelativeInputPath(argv, "fonts/arial.ttf"), font)) {
		cout << "OpenTypeFileInputTest: failed to read arial.ttf" << endl;
		return false;
	}

	// Act
	InputByteArrayStream stream((Byte*)&font[0], (LongFilePositionType)font.size());
	OpenTypeFileInput openType;
	EStatusCode status = openType.ReadOpenTypeFile(&stream, 0);

	// Assert
	bool ok = false;
	do {
		if(status != eSuccess) {
			cout << "OpenTypeFileInputTest: ReadOpenTypeFile rejected pristine arial.ttf" << endl;
			break;
		}
		if(openType.GetOpenTypeFontType() != EOpenTypeTrueType) {
			cout << "OpenTypeFileInputTest: expected EOpenTypeTrueType, got " << openType.GetOpenTypeFontType() << endl;
			break;
		}
		// arial.ttf (Microsoft Arial 7.00) has 3415 glyphs and full per-glyph hmtx coverage.
		if(openType.mMaxp.NumGlyphs != 3415) {
			cout << "OpenTypeFileInputTest: expected NumGlyphs=3415, got " << openType.mMaxp.NumGlyphs << endl;
			break;
		}
		if(openType.mHHea.NumberOfHMetrics != 3415) {
			cout << "OpenTypeFileInputTest: expected NumberOfHMetrics=3415, got " << openType.mHHea.NumberOfHMetrics << endl;
			break;
		}
		ok = true;
	} while(false);
	return ok;
}

// Exercises the name-table path: ReadName allocates mNameEntries with
// new NameTableEntry[count](), populates every entry, and FreeTables walks
// mNameEntriesCount deleting each .String. Value-initializing the array means
// .String is NULL before the populate loop assigns it, so the delete-walk is
// safe even if a future change leaves an entry unpopulated; the constructor
// now also zeroes mNameEntriesCount so the FreeTables loop bound is never an
// indeterminate value. Parse happens on a heap object that is deleted inside
// the test so the destructor's delete-walk runs under the test, not at
// process teardown. Asserts exact values (arial.ttf has 58 name entries;
// entry 1 is the family name "Arial" in UTF-16BE) so a regression that
// mispopulates the table surfaces rather than a bounds check passing.
static bool ReadName_ArialTtf_PopulatesNameEntries(char* argv[]) {
	// Arrange
	string font;
	if(!readFileBytes(BuildRelativeInputPath(argv, "fonts/arial.ttf"), font)) {
		cout << "OpenTypeFileInputTest: failed to read arial.ttf" << endl;
		return false;
	}

	// Act
	InputByteArrayStream stream((Byte*)&font[0], (LongFilePositionType)font.size());
	OpenTypeFileInput* openType = new OpenTypeFileInput();
	EStatusCode status = openType->ReadOpenTypeFile(&stream, 0);

	// Assert
	bool ok = false;
	do {
		if(status != eSuccess) {
			cout << "OpenTypeFileInputTest: ReadOpenTypeFile rejected pristine arial.ttf" << endl;
			break;
		}
		if(openType->mName.mNameEntriesCount != 58) {
			cout << "OpenTypeFileInputTest: expected 58 name entries, got " << openType->mName.mNameEntriesCount << endl;
			break;
		}
		bool entriesOk = true;
		for(unsigned short i = 0; i < openType->mName.mNameEntriesCount; ++i) {
			if(openType->mName.mNameEntries[i].String == NULL) {
				cout << "OpenTypeFileInputTest: name entry " << i << " has NULL String" << endl;
				entriesOk = false;
				break;
			}
		}
		if(!entriesOk)
			break;
		const NameTableEntry& family = openType->mName.mNameEntries[1];
		if(family.PlatformID != 0 || family.EncodingID != 3 || family.NameID != 1 || family.Length != 10) {
			cout << "OpenTypeFileInputTest: name entry 1 header mismatch (plat " << family.PlatformID
			     << " enc " << family.EncodingID << " name " << family.NameID
			     << " len " << family.Length << ")" << endl;
			break;
		}
		const char expectedArial[10] = {0,'A',0,'r',0,'i',0,'a',0,'l'};
		if(memcmp(family.String, expectedArial, 10) != 0) {
			cout << "OpenTypeFileInputTest: name entry 1 is not UTF-16BE \"Arial\"" << endl;
			break;
		}
		ok = true;
	} while(false);

	delete openType;
	return ok;
}

int OpenTypeFileInputTest(int argc, char* argv[]) {
	(void)argc;
	if(!ReadHMtx_NumberOfHMetricsZero_ReturnsFailure(argv)) return 1;
	if(!ReadOpenTypeSFNT_TtcfZeroOffsetTable_ReturnsFailure(argv)) return 1;
	if(!ReadName_StringRangeEscapesNameTable_ClampsEntry(argv)) return 1;
	if(!ReadGlyfForDependencies_LocaNotMonotonic_ReturnsFailure(argv)) return 1;
	if(!ReadOpenTypeFile_ArialTtf_PopulatesHheaMaxp(argv)) return 1;
	if(!ReadName_ArialTtf_PopulatesNameEntries(argv)) return 1;
	return 0;
}
