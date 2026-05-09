/*
   Source File : Type1PSTokensTest.cpp


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


   Regression tests for the Type 1 PostScript-token boundary helpers
   (Type1PSTokens::IsComment / FromPSName / FromPSString) extracted from
   Type1Input. Pre-extraction these were private members and pre-fix:

     * IsComment("")        -> std::out_of_range from at(0)
     * FromPSName("")       -> std::out_of_range from substr(1)
     * FromPSString("")     -> size()-1 underflows size_t to SIZE_MAX,
                               loop runs until ++it past end() segfaults
     * FromPSString("(\\01") -> truncated octal escape advances iterator
                                three positions past end(); *it reads OOB

   End-to-end coverage on a real PFB lives in Type1InputTest.
*/
#include "Type1PSTokens.h"

#include <iostream>
#include <string>

using namespace std;

// Pre-fix: at(0) on an empty string throws std::out_of_range, which
// the caller (the hot token loop in ReadType1File / ReadFontDictionary
// / ReadFontInfoDictionary / ParseCharstrings) does not catch. Empty
// tokens are reachable through the tokenizer's edge cases (e.g. an
// empty hex literal "<>"), so a malformed PFB can abort the process.
static bool IsComment_EmptyToken_ReturnsFalse() {
	// Act + Assert
	if(Type1PSTokens::IsComment(string())) {
		cout << "Type1PSTokensTest: IsComment(\"\") returned true" << endl;
		return false;
	}
	return true;
}

// Sanity: the empty-token guard didn't turn IsComment into a no-op.
static bool IsComment_PercentToken_ReturnsTrue() {
	// Act + Assert
	if(!Type1PSTokens::IsComment("% comment")) {
		cout << "Type1PSTokensTest: IsComment(\"% comment\") returned false" << endl;
		return false;
	}
	return true;
}

// Pre-fix: substr(1) on an empty string throws std::out_of_range. A
// crafted PFB with an empty token after /FontName / /Encoding /
// charstring name aborts the process.
static bool FromPSName_EmptyToken_ReturnsEmpty() {
	// Act + Assert
	if(!Type1PSTokens::FromPSName(string()).empty()) {
		cout << "Type1PSTokensTest: FromPSName(\"\") returned non-empty" << endl;
		return false;
	}
	return true;
}

// Sanity: the empty-token guard didn't break the leading-slash strip.
static bool FromPSName_SlashedName_StripsLeadingSlash() {
	// Act
	string result = Type1PSTokens::FromPSName("/HelveticaNeue-Bold");

	// Assert
	if(result != "HelveticaNeue-Bold") {
		cout << "Type1PSTokensTest: FromPSName(\"/HelveticaNeue-Bold\") returned \"" << result << "\"" << endl;
		return false;
	}
	return true;
}

// Pre-fix: the for loop bound `i < inPSString.size()-1` underflows
// size_t to SIZE_MAX when size()==0, and the loop runs until ++it
// past end() segfaults.
static bool FromPSString_Empty_ReturnsEmpty() {
	// Act + Assert
	if(!Type1PSTokens::FromPSString(string()).empty()) {
		cout << "Type1PSTokensTest: FromPSString(\"\") returned non-empty" << endl;
		return false;
	}
	return true;
}

// A single "(" is not a valid PostScript string literal (need both
// opening and closing parens). Pre-fix: size()-1 == 0, the loop never
// runs and the function silently returns "" -- but only by accident,
// since the underflow above was avoided. Post-fix: the size < 2 guard
// makes the empty return explicit.
static bool FromPSString_OneChar_ReturnsEmpty() {
	// Act + Assert
	if(!Type1PSTokens::FromPSString("(").empty()) {
		cout << "Type1PSTokensTest: FromPSString(\"(\") returned non-empty" << endl;
		return false;
	}
	return true;
}

// V-071 / V-2.3-020: Pre-fix the octal-escape branch did three
// unconditional ++it. With input "(\\01" the third ++it lands one
// position past end() and *it reads OOB. The bounded loop must abort
// the partial escape and produce a best-effort prefix without
// reading past end().
static bool FromPSString_TruncatedOctalEscape_DoesNotReadPastEnd() {
	// Act
	// Input bytes: '(', '\\', '0', '1'  -- closing paren missing, third
	// octal digit missing. The post-fix loop treats the final char ('1')
	// as the closing-paren sentinel and breaks out mid-escape; nothing
	// is emitted because the escape never completes.
	string result = Type1PSTokens::FromPSString("(\\01");

	// Assert
	if(!result.empty()) {
		cout << "Type1PSTokensTest: FromPSString(\"(\\\\01\") emitted \"" << result << "\"" << endl;
		return false;
	}
	return true;
}

// Sanity: a complete octal escape still decodes to the encoded byte.
// Plaintext "(\\101)" -> octal 101 == 'A'.
static bool FromPSString_OctalEscape_DecodesToByte() {
	// Act
	string result = Type1PSTokens::FromPSString("(\\101)");

	// Assert
	if(result != "A") {
		cout << "Type1PSTokensTest: FromPSString(\"(\\\\101)\") returned \"" << result << "\" (expected \"A\")" << endl;
		return false;
	}
	return true;
}

// Sanity: a plain string round-trips with no escape handling.
static bool FromPSString_PlainLiteral_RoundTrips() {
	// Act
	string result = Type1PSTokens::FromPSString("(Bold)");

	// Assert
	if(result != "Bold") {
		cout << "Type1PSTokensTest: FromPSString(\"(Bold)\") returned \"" << result << "\"" << endl;
		return false;
	}
	return true;
}

int Type1PSTokensTest(int argc, char* argv[]) {
	(void) argc;
	(void) argv;
	if(!IsComment_EmptyToken_ReturnsFalse()) return 1;
	if(!IsComment_PercentToken_ReturnsTrue()) return 1;
	if(!FromPSName_EmptyToken_ReturnsEmpty()) return 1;
	if(!FromPSName_SlashedName_StripsLeadingSlash()) return 1;
	if(!FromPSString_Empty_ReturnsEmpty()) return 1;
	if(!FromPSString_OneChar_ReturnsEmpty()) return 1;
	if(!FromPSString_TruncatedOctalEscape_DoesNotReadPastEnd()) return 1;
	if(!FromPSString_OctalEscape_DecodesToByte()) return 1;
	if(!FromPSString_PlainLiteral_RoundTrips()) return 1;
	return 0;
}
