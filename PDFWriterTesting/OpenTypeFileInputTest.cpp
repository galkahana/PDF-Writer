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

int OpenTypeFileInputTest(int argc, char* argv[]) {
	(void)argc;
	if(!ReadHMtx_NumberOfHMetricsZero_ReturnsFailure(argv)) return 1;
	if(!ReadOpenTypeFile_ArialTtf_PopulatesHheaMaxp(argv)) return 1;
	return 0;
}
