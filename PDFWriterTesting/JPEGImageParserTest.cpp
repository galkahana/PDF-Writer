/*
   Source File : JPEGImageParserTest.cpp


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


*/
#include "JPEGImageParser.h"
#include "JPEGImageInformation.h"
#include "InputByteArrayStream.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace PDFHummus;
using IOBasicTypes::Byte;

// ---- byte emitters ------------------------------------------------------

static void EmitU8(vector<Byte>& out, unsigned int v)
{
	out.push_back((Byte)(v & 0xff));
}
static void EmitU16BE(vector<Byte>& out, unsigned int v)
{
	EmitU8(out, (v >> 8) & 0xff);
	EmitU8(out, v & 0xff);
}
static void EmitU32BE(vector<Byte>& out, unsigned long v)
{
	EmitU16BE(out, (unsigned int)((v >> 16) & 0xffffUL));
	EmitU16BE(out, (unsigned int)(v & 0xffffUL));
}
static void EmitU16LE(vector<Byte>& out, unsigned int v)
{
	EmitU8(out, v & 0xff);
	EmitU8(out, (v >> 8) & 0xff);
}
static void EmitU32LE(vector<Byte>& out, unsigned long v)
{
	EmitU16LE(out, (unsigned int)(v & 0xffffUL));
	EmitU16LE(out, (unsigned int)((v >> 16) & 0xffffUL));
}

// SOI marker
static void EmitSOI(vector<Byte>& out)
{
	EmitU8(out, 0xff);
	EmitU8(out, 0xd8);
}

// Minimal SOF0: declared marker length, then height/width/components.
// markerLen counts the length field (2 bytes) plus payload. Real SOF0 needs
// >= 11 bytes (precision + h + w + nf + per-component triples); tests can pass
// shorter values to exercise the length-underflow guard.
static void EmitSOF0(vector<Byte>& out,
                     unsigned int markerLen,
                     unsigned int height,
                     unsigned int width,
                     unsigned int components)
{
	EmitU8(out, 0xff);
	EmitU8(out, 0xc0);
	EmitU16BE(out, markerLen);
	EmitU8(out, 8);              // precision
	EmitU16BE(out, height);
	EmitU16BE(out, width);
	EmitU8(out, components);
	// per-component triples (3 bytes each) so a real markerLen >= 11 is consistent
	for(unsigned int i = 0; i < components; ++i) {
		EmitU8(out, (unsigned int)(i + 1));
		EmitU8(out, 0x11);
		EmitU8(out, 0);
	}
}

// APP0 (JFIF) marker. Real markerLen is >= 16 in practice (incl 5-byte "JFIF\0"
// identifier and version), but the parser only requires >= 14 because it reads
// exactly 14 bytes of header. Tests may pass shorter values to exercise the
// guard.
static void EmitJFIF(vector<Byte>& out,
                     unsigned int markerLen,
                     unsigned int xDensity,
                     unsigned int yDensity)
{
	EmitU8(out, 0xff);
	EmitU8(out, 0xe0);
	EmitU16BE(out, markerLen);
	// "JFIF\0" identifier
	EmitU8(out, 'J'); EmitU8(out, 'F'); EmitU8(out, 'I'); EmitU8(out, 'F'); EmitU8(out, 0);
	EmitU8(out, 1);              // version major
	EmitU8(out, 2);              // version minor
	EmitU8(out, 1);              // units (1 = dots-per-inch)
	EmitU16BE(out, xDensity);
	EmitU16BE(out, yDensity);
	// thumbnail width/height (assumed 0)
	if(markerLen >= 16) {
		EmitU8(out, 0);
		EmitU8(out, 0);
	}
	// extra padding to reach markerLen
	unsigned int written = 14;
	if(markerLen >= 16) written = 16;
	while(written < markerLen) {
		EmitU8(out, 0);
		++written;
	}
}

// Single-byte APP14 (unknown to the parser) used to exercise SkipTag's
// length-underflow guard. markerLen of 1 is below the 2-byte length field
// itself; with the guard in place SkipTag must reject it.
static void EmitUnknownApp(vector<Byte>& out, unsigned int markerLen)
{
	EmitU8(out, 0xff);
	EmitU8(out, 0xee);           // APP14
	EmitU16BE(out, markerLen);
	// no payload — short-length is the whole point
}

// APP13 (Photoshop) with a declared marker length. Bytes after the length
// field aren't validated when markerLen < 2 because the guard fires before any
// payload read.
static void EmitPhotoshopShort(vector<Byte>& out, unsigned int markerLen)
{
	EmitU8(out, 0xff);
	EmitU8(out, 0xed);
	EmitU16BE(out, markerLen);
}

// ---- Exif builders ------------------------------------------------------

// Builds an APP1/Exif segment containing a 3-entry IFD: xResolution,
// yResolution, ResolutionUnit. The rational values for x/y resolution are
// placed immediately after the IFD entries. inXResOffset / inYResOffset
// override the offset value written into the IFD entry without changing the
// position of the actual rational data (used to provoke offset-arithmetic
// faults).
//
// inIfdOffset is the value written for the IFD0 offset; in well-formed input
// it equals 8 (IFD immediately follows the 8-byte TIFF header). Smaller
// values exercise the IFD-offset bound check.
//
// inRationalDenominator is the denominator written into each rational; 0
// exercises the divide-by-zero guard.
static void EmitExifSegment(vector<Byte>& out,
                            unsigned long inIfdOffset,
                            unsigned long inXResOffset,
                            unsigned long inYResOffset,
                            unsigned int  inResolutionUnit,
                            unsigned long inRationalNumerator,
                            unsigned long inRationalDenominator)
{
	// Big-endian Exif.
	const unsigned int xResTag        = 0x011a;
	const unsigned int yResTag        = 0x011b;
	const unsigned int resUnitTag     = 0x0128;
	const unsigned int tiffType_RATIONAL = 5;
	const unsigned int tiffType_SHORT    = 3;

	// Compute marker payload separately so we can write the size up front.
	vector<Byte> payload;

	// "Exif\0\0"
	EmitU8(payload, 'E'); EmitU8(payload, 'x'); EmitU8(payload, 'i'); EmitU8(payload, 'f');
	EmitU8(payload, 0);   EmitU8(payload, 0);

	// TIFF header — big endian.
	EmitU8(payload, 'M'); EmitU8(payload, 'M');
	EmitU16BE(payload, 0x002a);
	EmitU32BE(payload, inIfdOffset);

	// If the declared IFD offset > 8, pad the gap so the IFD lands where we
	// claimed.
	unsigned long padTo = inIfdOffset;
	if(padTo < 8) padTo = 8;            // bound-check tests still need a sane layout
	for(unsigned long i = 8; i < padTo; ++i) EmitU8(payload, 0);

	// IFD0: 3 entries.
	EmitU16BE(payload, 3);

	// Each entry: tag(2) type(2) count(4) value-or-offset(4) = 12 bytes.
	EmitU16BE(payload, xResTag);
	EmitU16BE(payload, tiffType_RATIONAL);
	EmitU32BE(payload, 1);
	EmitU32BE(payload, inXResOffset);

	EmitU16BE(payload, yResTag);
	EmitU16BE(payload, tiffType_RATIONAL);
	EmitU32BE(payload, 1);
	EmitU32BE(payload, inYResOffset);

	EmitU16BE(payload, resUnitTag);
	EmitU16BE(payload, tiffType_SHORT);
	EmitU32BE(payload, 1);
	// SHORT value packed into the high half of the 4-byte field for big endian.
	EmitU16BE(payload, inResolutionUnit);
	EmitU16BE(payload, 0);

	// Two rationals placed sequentially. Their absolute offsets from the TIFF
	// header start are (padTo + 2 + 3*12) and (padTo + 2 + 3*12 + 8).
	EmitU32BE(payload, inRationalNumerator);
	EmitU32BE(payload, inRationalDenominator);
	EmitU32BE(payload, inRationalNumerator);
	EmitU32BE(payload, inRationalDenominator);

	// Length field counts itself (2 bytes) + payload.
	unsigned int markerLen = (unsigned int)(2 + payload.size());

	EmitU8(out, 0xff);
	EmitU8(out, 0xe1);
	EmitU16BE(out, markerLen);
	out.insert(out.end(), payload.begin(), payload.end());
}

// ---- test runner --------------------------------------------------------

static EStatusCode RunParse(const vector<Byte>& inBytes, JPEGImageInformation& outInfo)
{
	// Arrange
	InputByteArrayStream stream(const_cast<Byte*>(inBytes.data()), (long long)inBytes.size());

	// Act
	JPEGImageParser parser;
	return parser.Parse(&stream, outInfo);
}

// ---- happy-path baseline ------------------------------------------------

static bool Parse_MinimalSOF0_Succeeds()
{
	// Arrange
	vector<Byte> bytes;
	EmitSOI(bytes);
	EmitSOF0(bytes, 11, 100, 200, 1);

	// Act
	JPEGImageInformation info;
	EStatusCode status = RunParse(bytes, info);

	// Assert
	if(status != eSuccess) {
		cout << "JPEGImageParserTest [Parse_MinimalSOF0_Succeeds]: status " << status
		     << ", expected " << eSuccess << endl;
		return false;
	}
	if(info.SamplesHeight != 100 || info.SamplesWidth != 200) {
		cout << "JPEGImageParserTest [Parse_MinimalSOF0_Succeeds]: dims "
		     << info.SamplesWidth << "x" << info.SamplesHeight
		     << ", expected 200x100" << endl;
		return false;
	}
	if(info.ColorComponentsCount != 1) {
		cout << "JPEGImageParserTest [Parse_MinimalSOF0_Succeeds]: components "
		     << info.ColorComponentsCount << ", expected 1" << endl;
		return false;
	}
	return true;
}

static bool Parse_JfifThenSOF0_RecordsDensity()
{
	// Arrange
	vector<Byte> bytes;
	EmitSOI(bytes);
	EmitJFIF(bytes, 16, 72, 96);
	EmitSOF0(bytes, 11, 50, 80, 3);

	// Act
	JPEGImageInformation info;
	EStatusCode status = RunParse(bytes, info);

	// Assert
	if(status != eSuccess) {
		cout << "JPEGImageParserTest [Parse_JfifThenSOF0_RecordsDensity]: status " << status << endl;
		return false;
	}
	if(!info.JFIFInformationExists) {
		cout << "JPEGImageParserTest [Parse_JfifThenSOF0_RecordsDensity]: "
		        "JFIFInformationExists not set" << endl;
		return false;
	}
	if(info.JFIFXDensity != 72 || info.JFIFYDensity != 96) {
		cout << "JPEGImageParserTest [Parse_JfifThenSOF0_RecordsDensity]: density "
		     << info.JFIFXDensity << "x" << info.JFIFYDensity
		     << ", expected 72x96" << endl;
		return false;
	}
	return true;
}

// ---- length-underflow cluster ------------------------------------------

// Each short-marker case declares a marker length below the parser's
// fixed-size header read. Pre-fix code would write attacker-controlled values
// into JPEGImageInformation BEFORE noticing the length was wrong (the
// underflow then drove a wild SkipStream call). With the guards in place
// each per-marker reader rejects the short read before any output field is
// written, and Parse propagates the failure: SOFMarkerNotFound is now only
// cleared on a successful ReadSOF0Data, so a malformed SOF0 keeps the flag
// raised and Parse returns eFailure. The other short-marker cases never
// reach a valid SOF, so they also fail.

static bool IsJpegInfoAtDefaults(const JPEGImageInformation& info)
{
	return info.SamplesWidth == 0
	    && info.SamplesHeight == 0
	    && info.ColorComponentsCount == 0
	    && !info.JFIFInformationExists
	    && info.JFIFXDensity == 0 && info.JFIFYDensity == 0
	    && !info.ExifInformationExists
	    && info.ExifXDensity == 0 && info.ExifYDensity == 0
	    && !info.PhotoshopInformationExists
	    && info.PhotoshopXDensity == 0 && info.PhotoshopYDensity == 0;
}

struct ShortMarkerCase {
	const char* label;
	void (*buildAfterSOI)(vector<Byte>&);
};

// SOF0 with declared length 7 — below the 8-byte fixed header read. Pre-fix
// code would still copy SamplesHeight=100 and SamplesWidth=200 from the read
// buffer.
static void BuildShortSOF0(vector<Byte>& out)
{
	EmitSOF0(out, /*markerLen*/ 7, /*h*/ 100, /*w*/ 200, /*comp*/ 1);
}
// JFIF with declared length 13 — below the 14-byte fixed header read. Pre-fix
// code would set JFIFInformationExists=true and write the densities.
static void BuildShortJFIF(vector<Byte>& out)
{
	EmitJFIF(out, /*markerLen*/ 13, /*xDen*/ 72, /*yDen*/ 96);
}
// Photoshop / SkipTag short markers don't surface in observable info fields
// even pre-fix; the guard's value is reaching this assertion at all (rather
// than driving a wild skip).
static void BuildShortPhotoshop(vector<Byte>& out) { EmitPhotoshopShort(out, /*markerLen*/ 1); }
static void BuildShortUnknownApp(vector<Byte>& out){ EmitUnknownApp     (out, /*markerLen*/ 1); }

static const ShortMarkerCase scShortMarkerCases[] = {
	{"ShortSOF0_DoesNotRecordDimensions",     BuildShortSOF0},
	{"ShortJFIF_DoesNotRecordDensity",        BuildShortJFIF},
	{"ShortPhotoshop_LeavesInfoAtDefaults",   BuildShortPhotoshop},
	{"ShortSkipTag_LeavesInfoAtDefaults",     BuildShortUnknownApp},
};

static bool RunShortMarkerCases()
{
	const size_t count = sizeof(scShortMarkerCases) / sizeof(scShortMarkerCases[0]);
	for(size_t i = 0; i < count; ++i) {
		const ShortMarkerCase& c = scShortMarkerCases[i];

		// Arrange
		vector<Byte> bytes;
		EmitSOI(bytes);
		c.buildAfterSOI(bytes);

		// Act
		JPEGImageInformation info;
		EStatusCode status = RunParse(bytes, info);

		// Assert — Parse must report failure (no valid SOF reached) and the
		// info struct must stay at defaults.
		if(status == eSuccess) {
			cout << "JPEGImageParserTest [ShortMarker::" << c.label
			     << "]: Parse returned eSuccess; expected eFailure" << endl;
			return false;
		}
		if(!IsJpegInfoAtDefaults(info)) {
			cout << "JPEGImageParserTest [ShortMarker::" << c.label
			     << "]: info struct was mutated despite short marker length "
			        "(dims=" << info.SamplesWidth << "x" << info.SamplesHeight
			     << " jfif=" << info.JFIFInformationExists
			     << " exif=" << info.ExifInformationExists
			     << " psd="  << info.PhotoshopInformationExists << ")" << endl;
			return false;
		}
	}
	return true;
}

// ---- Exif-path cases ---------------------------------------------------

// Exif failure is treated by Parse as "not Exif" and the marker is ignored
// (ExifMarkerNotFound is reset to true). Every case below pairs an Exif
// segment with a valid SOF0 so Parse's outer status stays eSuccess and the
// SOF0 dimensions become a sanity check that the post-Exif drain didn't
// over-skip the stream.
struct ExifCase {
	const char* label;
	// input — fed into EmitExifSegment
	unsigned long ifdOffset;
	unsigned long xResOff;
	unsigned long yResOff;
	unsigned long rationalNumerator;
	unsigned long rationalDenominator;
	// expected outcome
	bool          expectExifInfoSet;     // ExifInformationExists after parse
	double        expectedXDensity;
	double        expectedYDensity;
};

// Offset to the first rational when ifdOffset=8 (no TIFF-header padding) and
// the IFD has 3 entries placed immediately after the IFD entry count: 8 (TIFF
// header) + 2 (entry count) + 3*12 (entries) = 46.
static const unsigned long scRational0Off = 46;
static const unsigned long scRational1Off = 54; // 46 + 8

static const ExifCase scExifCases[] = {
	// Well-formed baseline: pins the happy path mapping (300/2 = 150).
	{"WellFormed_RecordsDensity",
	 /*ifdOffset*/ 8, /*xRes*/ scRational0Off, /*yRes*/ scRational1Off,
	 /*num*/ 300, /*den*/ 2,
	 /*exifSet*/ true, /*x*/ 150.0, /*y*/ 150.0},

	// IFD0 offset below the 8-byte TIFF header minimum — rejected at V-093
	// guard, BEFORE ExifInformationExists is set.
	{"IfdOffsetBelowEight_IgnoredAsXMP",
	 /*ifdOffset*/ 4, /*xRes*/ scRational0Off, /*yRes*/ scRational1Off,
	 /*num*/ 72, /*den*/ 1,
	 /*exifSet*/ false, /*x*/ 0.0, /*y*/ 0.0},

	// xResolution IFD-entry offset (10) precedes parser position after IFD
	// loop (46) — rejected at GetResolutionFromExif's ordering guard, AFTER
	// ExifInformationExists is set.
	{"ResolutionOffsetBeforeIfd_IgnoredAsXMP",
	 /*ifdOffset*/ 8, /*xRes*/ 10, /*yRes*/ scRational1Off,
	 /*num*/ 72, /*den*/ 1,
	 /*exifSet*/ true, /*x*/ 0.0, /*y*/ 0.0},

	// Valid offsets, denominator=0 — rejected at ReadRationalValue, AFTER
	// ExifInformationExists is set.
	{"RationalDenominatorZero_IgnoredAsXMP",
	 /*ifdOffset*/ 8, /*xRes*/ scRational0Off, /*yRes*/ scRational1Off,
	 /*num*/ 72, /*den*/ 0,
	 /*exifSet*/ true, /*x*/ 0.0, /*y*/ 0.0},
};

static bool RunExifCases()
{
	const size_t count = sizeof(scExifCases) / sizeof(scExifCases[0]);
	for(size_t i = 0; i < count; ++i) {
		const ExifCase& c = scExifCases[i];

		// Arrange
		vector<Byte> bytes;
		EmitSOI(bytes);
		EmitExifSegment(bytes, c.ifdOffset, c.xResOff, c.yResOff,
		                /*resUnit*/ 2, c.rationalNumerator, c.rationalDenominator);
		EmitSOF0(bytes, 11, /*h*/ 100, /*w*/ 200, /*comp*/ 1);

		// Act
		JPEGImageInformation info;
		EStatusCode status = RunParse(bytes, info);

		// Assert — overall parse, SOF0 reached, Exif fields match expectation.
		if(status != eSuccess) {
			cout << "JPEGImageParserTest [Exif::" << c.label
			     << "]: status " << status << ", expected eSuccess" << endl;
			return false;
		}
		if(info.SamplesWidth != 200 || info.SamplesHeight != 100) {
			cout << "JPEGImageParserTest [Exif::" << c.label
			     << "]: SOF0 dims " << info.SamplesWidth << "x" << info.SamplesHeight
			     << ", expected 200x100 (drain must end at marker boundary)" << endl;
			return false;
		}
		if(info.ExifInformationExists != c.expectExifInfoSet) {
			cout << "JPEGImageParserTest [Exif::" << c.label
			     << "]: ExifInformationExists=" << info.ExifInformationExists
			     << ", expected " << c.expectExifInfoSet << endl;
			return false;
		}
		if(info.ExifXDensity != c.expectedXDensity || info.ExifYDensity != c.expectedYDensity) {
			cout << "JPEGImageParserTest [Exif::" << c.label
			     << "]: density " << info.ExifXDensity << "x" << info.ExifYDensity
			     << ", expected " << c.expectedXDensity << "x" << c.expectedYDensity << endl;
			return false;
		}
	}
	return true;
}

// Silence unused-warning for the little-endian emitters; they exist so future
// Exif fixtures can flip endianness without touching the builder.
static void EnsureEmittersReferenced()
{
	vector<Byte> tmp;
	EmitU16LE(tmp, 0);
	EmitU32LE(tmp, 0);
	(void)tmp;
}

int JPEGImageParserTest(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	EnsureEmittersReferenced();

	if(!Parse_MinimalSOF0_Succeeds())        return 1;
	if(!Parse_JfifThenSOF0_RecordsDensity()) return 1;
	if(!RunShortMarkerCases())               return 1;
	if(!RunExifCases())                      return 1;
	return 0;
}
