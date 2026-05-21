/*
   Source File : FreeTypeFaceWrapperTest.cpp


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


   Regression tests for FreeTypeFaceWrapper's FreeType return-code handling:

     * V-076: SelectDefaultPalette assigned FT_Error to bool and ANDed the
       two results, so a single failing FT_Palette_* call reported success;
       it then read num_palette_entries out of an uninitialized
       FT_Palette_Data and handed a garbage palette size to the caller.
     * V-077: GetGlyphOutline tested mFace->glyph->format BEFORE LoadGlyph
       (gating on the stale shared slot - a fresh face returns false for a
       perfectly valid outline glyph) and then unconditionally overwrote the
       FT_Outline_Decompose result with status = true.
     * V-078: GetGlyphName built std::string(buffer) from an uninitialized
       char[100] when FT_Get_Glyph_Name failed (no post table / post format
       3 / color fonts), leaking stack bytes into the PDF.
*/
#include "FreeTypeFaceWrapper.h"
#include "FreeTypeWrapper.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// Counts the path callbacks FreeTypeFaceWrapper::GetGlyphOutline drives
// through FT_Outline_Decompose. Subclasses the wrapper's enumerator
// interface; the protected overrides are reached via the static FT
// trampolines that GetGlyphOutline is friended to.
class CountingOutlineEnumerator : public FreeTypeFaceWrapper::IOutlineEnumerator {
public:
	CountingOutlineEnumerator() : mMoveto(0), mLineto(0), mCurveto(0), mClose(0) {}
	unsigned int mMoveto;
	unsigned int mLineto;
	unsigned int mCurveto;
	unsigned int mClose;
protected:
	virtual bool Moveto(FT_Short, FT_Short) { ++mMoveto; return true; }
	virtual bool Lineto(FT_Short, FT_Short) { ++mLineto; return true; }
	virtual bool Curveto(FT_Short, FT_Short, FT_Short, FT_Short, FT_Short, FT_Short) { ++mCurveto; return true; }
	virtual bool Close() { ++mClose; return true; }
};

// V-077: build a wrapper on a brand-new face (no glyph ever loaded into the
// shared slot) and ask for a valid outline glyph as the very first
// operation. Pre-fix the format gate ran before LoadGlyph and saw
// FT_GLYPH_FORMAT_NONE, so this returned false for a real glyph; the
// unconditional status = true also masked any decompose failure. Post-fix
// it loads, sees FT_GLYPH_FORMAT_OUTLINE, decomposes, and reports the real
// result - a valid 'A' must yield true and emit at least one contour.
static bool GetGlyphOutline_FreshFaceValidOutlineGlyph_ReturnsTrueAndEmitsPath(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphOutline::FreshFaceValidOutlineGlyph_ReturnsTrueAndEmitsPath]: failed to load arial.ttf" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face, "", 0, false);
		FT_UInt glyphIndex = FT_Get_Char_Index(face, 'A');
		if(glyphIndex == 0) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphOutline::FreshFaceValidOutlineGlyph_ReturnsTrueAndEmitsPath]: no glyph for 'A'" << endl;
			break;
		}
		CountingOutlineEnumerator enumerator;

		// Act
		bool status = wrapper.GetGlyphOutline(glyphIndex, enumerator);

		// Assert
		if(!status) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphOutline::FreshFaceValidOutlineGlyph_ReturnsTrueAndEmitsPath]: returned false for a valid outline glyph on a fresh face" << endl;
			break;
		}
		if(enumerator.mMoveto == 0) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphOutline::FreshFaceValidOutlineGlyph_ReturnsTrueAndEmitsPath]: no Moveto emitted - the outline was not decomposed" << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

// V-078 happy path: a font with a real post table (arial.ttf, post format
// 2) must still return the correct glyph name. Locks in that the added
// return-value check did not regress the success path.
static bool GetGlyphName_FontWithGlyphNames_ReturnsExactName(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphName::FontWithGlyphNames_ReturnsExactName]: failed to load arial.ttf" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face, "", 0, false);
		FT_UInt glyphIndex = FT_Get_Char_Index(face, 'A');
		if(glyphIndex == 0) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphName::FontWithGlyphNames_ReturnsExactName]: no glyph for 'A'" << endl;
			break;
		}

		// Act
		string name = wrapper.GetGlyphName(glyphIndex);

		// Assert
		if(name != "A") {
			cout << "FreeTypeFaceWrapperTest [GetGlyphName::FontWithGlyphNames_ReturnsExactName]: expected \"A\", got \"" << name << "\"" << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

// V-078 regression: noto-glyf_colr_1.ttf has post format 3, so it carries
// no glyph names and FT_Get_Glyph_Name fails for every glyph. Pre-fix the
// ignored return value left buffer[100] uninitialized and std::string()
// walked it (nondeterministic stack leak). Post-fix the failure is detected
// and GetGlyphName falls back to the deterministic .notdef name.
static bool GetGlyphName_FontWithoutGlyphNames_ReturnsNotDef(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/noto-glyf_colr_1.ttf"), 0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphName::FontWithoutGlyphNames_ReturnsNotDef]: failed to load noto-glyf_colr_1.ttf" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face, "", 0, false);
		if(face->num_glyphs < 2) {
			cout << "FreeTypeFaceWrapperTest [GetGlyphName::FontWithoutGlyphNames_ReturnsNotDef]: unexpected glyph count" << endl;
			break;
		}

		// Act
		// glyph index 1 is in range (num_glyphs > 1) but the font has no
		// names, so FT_Get_Glyph_Name fails for it
		string name = wrapper.GetGlyphName(1);

		// Assert
		if(name != ".notdef") {
			cout << "FreeTypeFaceWrapperTest [GetGlyphName::FontWithoutGlyphNames_ReturnsNotDef]: expected \".notdef\", got \"" << name << "\"" << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

// V-076 regression: arial.ttf has no CPAL table, so FT_Palette_Data_Get
// fails. Pre-fix *outPaletteSize was read from an uninitialized
// FT_Palette_Data member (nondeterministic). Post-fix a failed palette
// selection returns a nonzero FT_Error and zeroes both outputs.
static bool SelectDefaultPalette_FontWithoutPalette_ReturnsErrorAndZeroedOutputs(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithoutPalette_ReturnsErrorAndZeroedOutputs]: failed to load arial.ttf" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face, "", 0, false);
		FT_Color* palette = (FT_Color*)0x1; // poison: must be overwritten to NULL
		unsigned short paletteSize = 0xBEEF; // poison: must be overwritten to 0

		// Act
		FT_Error error = wrapper.SelectDefaultPalette(&palette, &paletteSize);

		// Assert
		if(error == FT_Err_Ok) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithoutPalette_ReturnsErrorAndZeroedOutputs]: expected a nonzero FT_Error for a font without a CPAL table" << endl;
			break;
		}
		if(palette != NULL) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithoutPalette_ReturnsErrorAndZeroedOutputs]: outPalette not nulled on failure" << endl;
			break;
		}
		if(paletteSize != 0) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithoutPalette_ReturnsErrorAndZeroedOutputs]: outPaletteSize not zeroed on failure, got " << paletteSize << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

// V-076 happy path: noto-glyf_colr_1.ttf carries a CPAL table with exactly
// 5879 palette entries. Both FT_Palette_* calls succeed, so the wrapper
// returns FT_Err_Ok with a non-NULL palette and the exact entry count -
// proving the corrected error semantics didn't break the success path.
static bool SelectDefaultPalette_FontWithPalette_ReturnsOkAndExactEntryCount(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/noto-glyf_colr_1.ttf"), 0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithPalette_ReturnsOkAndExactEntryCount]: failed to load noto-glyf_colr_1.ttf" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face, "", 0, false);
		FT_Color* palette = NULL;
		unsigned short paletteSize = 0;

		// Act
		FT_Error error = wrapper.SelectDefaultPalette(&palette, &paletteSize);

		// Assert
		if(error != FT_Err_Ok) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithPalette_ReturnsOkAndExactEntryCount]: expected FT_Err_Ok for a CPAL font, got " << error << endl;
			break;
		}
		if(palette == NULL) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithPalette_ReturnsOkAndExactEntryCount]: outPalette is NULL on success" << endl;
			break;
		}
		if(paletteSize != 5879) {
			cout << "FreeTypeFaceWrapperTest [SelectDefaultPalette::FontWithPalette_ReturnsOkAndExactEntryCount]: expected 5879 palette entries, got " << paletteSize << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

// V-079: four Adobe-Standard-Latin ranges in IsCharachterCodeAdobeStandard
// had their bounds reversed (e.g. betweenIncluding(c,0x2C6,0x1C7)), so the
// lo>hi test was always false and these accented/punctuation codepoints
// were misclassified as non-standard -> wrong IsSymbolic -> wrong PDF
// FontDescriptor /Flags. Each codepoint below must classify as Adobe
// Standard; pre-fix every one returned false. A CJK codepoint that is
// genuinely outside the set still returns false, proving the corrected
// ranges are not blanket-true.
static bool IsCharachterCodeAdobeStandard_PreviouslyReversedRanges_ReturnsTrue(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
	do {
		// Arrange
		if(!face) {
			cout << "FreeTypeFaceWrapperTest [IsCharachterCodeAdobeStandard::PreviouslyReversedRanges_ReturnsTrue]: failed to load arial.ttf" << endl;
			break;
		}
		FreeTypeFaceWrapper wrapper(face, "", 0, false);

		// circumflex/caron, ring/ogonek, breve/dotaccent, dagger/daggerdbl/bullet
		const FT_ULong inRange[] = { 0x2C6, 0x2C7, 0x2DA, 0x2DB, 0x2D8, 0x2D9,
		                             0x2020, 0x2021, 0x2022 };
		const size_t inRangeCount = sizeof(inRange) / sizeof(inRange[0]);

		// Act + Assert
		bool allStandard = true;
		for(size_t i = 0; i < inRangeCount; ++i) {
			if(!wrapper.IsCharachterCodeAdobeStandard(inRange[i])) {
				cout << "FreeTypeFaceWrapperTest [IsCharachterCodeAdobeStandard::PreviouslyReversedRanges_ReturnsTrue]: 0x"
				     << hex << inRange[i] << dec << " misclassified as non-standard (reversed-bound range)" << endl;
				allStandard = false;
			}
		}
		if(!allStandard)
			break;
		if(wrapper.IsCharachterCodeAdobeStandard(0x4E00)) {
			cout << "FreeTypeFaceWrapperTest [IsCharachterCodeAdobeStandard::PreviouslyReversedRanges_ReturnsTrue]: CJK U+4E00 wrongly classified as Adobe Standard" << endl;
			break;
		}
		ok = true;
	} while(false);

	ft.DoneFace(face);
	return ok;
}

int FreeTypeFaceWrapperTest(int argc, char* argv[]) {
	(void)argc;

	if(!GetGlyphOutline_FreshFaceValidOutlineGlyph_ReturnsTrueAndEmitsPath(argv)) return 1;
	if(!GetGlyphName_FontWithGlyphNames_ReturnsExactName(argv)) return 1;
	if(!GetGlyphName_FontWithoutGlyphNames_ReturnsNotDef(argv)) return 1;
	if(!SelectDefaultPalette_FontWithoutPalette_ReturnsErrorAndZeroedOutputs(argv)) return 1;
	if(!SelectDefaultPalette_FontWithPalette_ReturnsOkAndExactEntryCount(argv)) return 1;
	if(!IsCharachterCodeAdobeStandard_PreviouslyReversedRanges_ReturnsTrue(argv)) return 1;
	return 0;
}
