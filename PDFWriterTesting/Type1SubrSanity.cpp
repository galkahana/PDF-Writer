/*
   Source File : Type1SubrSanity.cpp


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


   Regression test for the Type 1 Subrs OOB primitives fixed in cluster C2
   (V-052/053/054/056). Each malformed PFB below would previously corrupt
   the heap or read OOB during ParseSubrs / GetSubr; with the fix in place
   ReadType1File rejects them with eFailure (no crash) and GetSubr returns
   NULL for out-of-range indices.

   The malicious PFBs are synthesised in-process: the eexec stream cipher
   is trivially mirrored from InputPFBDecodeStream so we can produce binary
   segments whose decoded plaintext is whatever payload the test wants.
*/
#include "InputByteArrayStream.h"
#include "Type1Input.h"
#include "InputFile.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;
using namespace IOBasicTypes;

// Mirror of InputPFBDecodeStream::DecodeByte used to *encode* a plaintext
// stream so the decoder will recover it byte-for-byte.
static void EexecEncryptByte(Byte inPlain, unsigned short& ioRandomizer, string& ioCipher) {
	Byte cipher = (Byte)(inPlain ^ (ioRandomizer >> 8));
	ioCipher.push_back((char)cipher);
	ioRandomizer = (unsigned short)(((cipher + ioRandomizer) * 52845 + 22719) % 65536);
}

static string EexecEncrypt(const string& inPlaintext) {
	unsigned short randomizer = 55665;
	string cipher;
	// Decoder discards the first 4 bytes after running them through the
	// random state. Plaintext value is irrelevant; pick spaces so the
	// pre-cipher bytes are printable.
	for(int i = 0; i < 4; ++i)
		EexecEncryptByte((Byte)' ', randomizer, cipher);
	for(size_t i = 0; i < inPlaintext.size(); ++i)
		EexecEncryptByte((Byte)(unsigned char)inPlaintext[i], randomizer, cipher);
	return cipher;
}

static void AppendU32LE(string& ioOut, unsigned int inValue) {
	ioOut.push_back((char)(inValue & 0xff));
	ioOut.push_back((char)((inValue >> 8) & 0xff));
	ioOut.push_back((char)((inValue >> 16) & 0xff));
	ioOut.push_back((char)((inValue >> 24) & 0xff));
}

static void AppendSegmentHeader(string& ioOut, Byte inType, unsigned int inLength) {
	ioOut.push_back((char)0x80);
	ioOut.push_back((char)inType);
	AppendU32LE(ioOut, inLength);
}

// Compose a minimal PFB: ASCII header (type 1) + encrypted body (type 2) + EOF (type 3).
// The ASCII head supplies the "begin"/"end" pair so ReadFontDictionary completes
// trivially; the encrypted body carries /Private and /Subrs so ReadPrivateDictionary
// reaches ParseSubrs, where the bug used to fire.
static string BuildMalformedPFB(const string& inEncryptedPayload) {
	const string asciiHead = "begin\nend\ncurrentfile eexec\n";
	const string cipher = EexecEncrypt(inEncryptedPayload);

	string pfb;
	AppendSegmentHeader(pfb, 1, (unsigned int)asciiHead.size());
	pfb += asciiHead;
	AppendSegmentHeader(pfb, 2, (unsigned int)cipher.size());
	pfb += cipher;
	pfb.push_back((char)0x80);
	pfb.push_back((char)0x03);
	return pfb;
}

static EStatusCode parseSyntheticPFB(const string& inPlaintext) {
	string pfb = BuildMalformedPFB(inPlaintext);
	// &pfb[0] returns char* (non-const) on a non-const string in every
	// supported standard; pfb.data() is const char* pre-C++17 and would
	// require a const-stripping cast.
	InputByteArrayStream stream((Byte*)&pfb[0], (LongFilePositionType)pfb.size());
	Type1Input type1;
	return type1.ReadType1File(&stream);
}

static EStatusCode parseRealPFB(char* argv[], Type1Input& outType1) {
	InputFile pfb;
	if(pfb.OpenFile(BuildRelativeInputPath(argv, "fonts/HLB_____.PFB")) != eSuccess)
		return eFailure;
	EStatusCode status = outType1.ReadType1File(pfb.GetInputStream());
	pfb.CloseFile();
	return status;
}

// V-054: huge /Subrs count would drive new Type1CharString[N] to OOM/giant-alloc.
static bool parsingHugeSubrsCount_returnsFailure() {
	// Arrange
	const string payload = "/Private\n/Subrs 999999 array\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: huge /Subrs count was accepted (V-054 regressed)" << endl;
		return false;
	}
	return true;
}

// V-052: subr index >= mSubrsCount used to write a Type1CharString OOB into mSubrs.
static bool parsingOutOfRangeSubrIndex_returnsFailure() {
	// Arrange
	const string payload = "/Private\n/Subrs 5 array\ndup 999999 4 RD test NP\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: OOB subr index was accepted (V-052 regressed)" << endl;
		return false;
	}
	return true;
}

// V-053: huge CodeLength widens to a giant size_t in new Byte[N] -> bad_alloc / DoS.
static bool parsingHugeSubrCodeLength_returnsFailure() {
	// Arrange
	const string payload = "/Private\n/Subrs 5 array\ndup 0 99999999 RD test NP\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: huge CodeLength was accepted (V-053 regressed)" << endl;
		return false;
	}
	return true;
}

// V-053: negative CodeLength widens the same way as the huge case.
static bool parsingNegativeSubrCodeLength_returnsFailure() {
	// Arrange
	const string payload = "/Private\n/Subrs 5 array\ndup 0 -1 RD test NP\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: negative CodeLength was accepted (V-053 regressed)" << endl;
		return false;
	}
	return true;
}

// A second /Subrs whose count is out of range used to leave mSubrs pointing
// at the first /Subrs's array while resetting mSubrsCount, so FreeTables
// would skip per-subr Code cleanup and leak. With the do-while-false +
// FreeTables-on-failure pattern in place, ParseSubrs must reject and leave
// no dangling state. We can only assert "didn't crash + parse failed" here;
// the leak itself is invisible without a sanitizer, but the cleanup is what
// makes the no-crash outcome possible at all.
static bool parsingDuplicateSubrsWithBadSecondCount_returnsFailure() {
	// Arrange
	// "ND" terminator on the last subr is load-bearing: ParseSubrs's per-subr
	// "skip till next line or array end" loop will otherwise consume the
	// trailing /Subrs token, hiding the duplicate from ReadPrivateDictionary.
	const string payload =
		"/Private\n"
		"/Subrs 2 array\n"
		"dup 0 4 RD test NP\n"
		"dup 1 4 RD test ND\n"
		"/Subrs 999999 array\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: duplicate /Subrs with bad second count was accepted" << endl;
		return false;
	}
	return true;
}

// ParseCharstrings now mirrors ParseSubrs's do-while-false + FreeCharStrings-on-failure
// pattern. Malformed CodeLength must be rejected without leaving partial charstrings.
static bool parsingMalformedCharstring_returnsFailure() {
	// Arrange
	const string payload =
		"/Private\n"
		"/CharStrings 1 dict dup begin\n"
		"/A 99999999 RD test ND\n"
		"end\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: malformed charstring CodeLength was accepted" << endl;
		return false;
	}
	return true;
}

// std::map::insert is no-op on duplicate keys, so without an explicit check
// we would silently leak the second charstring's Code allocation. The fix
// rejects the duplicate so the outer cleanup runs.
static bool parsingDuplicateCharstringName_returnsFailure() {
	// Arrange
	const string payload =
		"/Private\n"
		"/CharStrings 2 dict dup begin\n"
		"/A 4 RD test ND\n"
		"/A 4 RD test ND\n"
		"end\n";

	// Act
	EStatusCode status = parseSyntheticPFB(payload);

	// Assert
	if(status == eSuccess) {
		cout << "Type1SubrSanity: duplicate charstring name was accepted" << endl;
		return false;
	}
	return true;
}

// V-352 lesson: prove the fix didn't break the happy path, not just that it didn't crash.
static bool parsingValidPFB_returnsUsableSubrAtValidIndex(char* argv[]) {
	// Arrange
	Type1Input type1;
	if(parseRealPFB(argv, type1) != eSuccess) {
		cout << "Type1SubrSanity: positive path failed - real PFB rejected" << endl;
		return false;
	}

	// Act
	Type1CharString* subr = type1.GetSubr(0);

	// Assert
	if(subr == NULL || subr->Code == NULL || subr->CodeLength <= 0) {
		cout << "Type1SubrSanity: GetSubr(0) returned unusable subr on a real PFB" << endl;
		return false;
	}
	return true;
}

// V-056: GetSubr previously returned mSubrs + inSubrIndex without checking inSubrIndex < 0,
// yielding a wild pointer that the interpreter would dereference.
static bool getSubrWithNegativeIndex_returnsNull(char* argv[]) {
	// Arrange
	Type1Input type1;
	if(parseRealPFB(argv, type1) != eSuccess) {
		cout << "Type1SubrSanity: could not parse real PFB for GetSubr negative-index check" << endl;
		return false;
	}

	// Act
	Type1CharString* subr = type1.GetSubr(-1);

	// Assert
	if(subr != NULL) {
		cout << "Type1SubrSanity: GetSubr(-1) returned non-NULL (V-056 regressed)" << endl;
		return false;
	}
	return true;
}

static bool getSubrWithIndexBeyondCount_returnsNull(char* argv[]) {
	// Arrange
	Type1Input type1;
	if(parseRealPFB(argv, type1) != eSuccess) {
		cout << "Type1SubrSanity: could not parse real PFB for GetSubr OOB check" << endl;
		return false;
	}

	// Act
	Type1CharString* subr = type1.GetSubr(0x7fffffff);

	// Assert
	if(subr != NULL) {
		cout << "Type1SubrSanity: GetSubr(huge) returned non-NULL" << endl;
		return false;
	}
	return true;
}

int Type1SubrSanity(int argc, char* argv[]) {
	if(!parsingHugeSubrsCount_returnsFailure()) return 1;
	if(!parsingOutOfRangeSubrIndex_returnsFailure()) return 1;
	if(!parsingHugeSubrCodeLength_returnsFailure()) return 1;
	if(!parsingNegativeSubrCodeLength_returnsFailure()) return 1;
	if(!parsingDuplicateSubrsWithBadSecondCount_returnsFailure()) return 1;
	if(!parsingMalformedCharstring_returnsFailure()) return 1;
	if(!parsingDuplicateCharstringName_returnsFailure()) return 1;
	if(!parsingValidPFB_returnsUsableSubrAtValidIndex(argv)) return 1;
	if(!getSubrWithNegativeIndex_returnsNull(argv)) return 1;
	if(!getSubrWithIndexBeyondCount_returnsNull(argv)) return 1;
	return 0;
}
