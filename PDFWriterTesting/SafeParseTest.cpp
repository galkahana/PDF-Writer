/*
   Source File : SafeParseTest.cpp


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
#include "SafeParse.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// ---- TryParse<T> ---------------------------------------------------------

template <typename T>
struct ParseCase {
	const char* label;         // <Condition>_<Result>
	const char* input;
	bool        expectedOk;
	T           expectedValue; // only consulted when expectedOk == true
};

template <typename T>
static bool RunParseCases(const char* inTypeLabel,
                          const ParseCase<T>* inCases,
                          size_t inCount) {
	for(size_t i = 0; i < inCount; ++i) {
		const ParseCase<T>& testCase = inCases[i];

		// Arrange
		T outValue = T();

		// Act
		bool ok = TryParse(string(testCase.input), outValue);

		// Assert
		if(ok != testCase.expectedOk) {
			cout << "SafeParseTest [" << inTypeLabel << "::" << testCase.label
			     << "]: return " << ok << ", expected " << testCase.expectedOk << endl;
			return false;
		}
		if(ok && outValue != testCase.expectedValue) {
			cout << "SafeParseTest [" << inTypeLabel << "::" << testCase.label
			     << "]: outValue " << outValue
			     << ", expected " << testCase.expectedValue << endl;
			return false;
		}
	}
	return true;
}

static const ParseCase<int> scIntCases[] = {
	{"PositiveInteger_Succeeds",            "42",    true,  42},
	{"NegativeInteger_Succeeds",            "-17",   true,  -17},
	{"TrailingNonNumeric_SucceedsLenient",  "12abc", true,  12},
	{"NonNumeric_Fails",                    "ABC",   false, 0},
	{"EmptyString_Fails",                   "",      false, 0},
};

// One row, just to prove TryParse instantiates for double and extracts as a
// real (3.14, not 3-with-dot-as-trailing-garbage). The shared template body
// is otherwise covered by the int table.
static const ParseCase<double> scDoubleCases[] = {
	{"DecimalNumber_ParsesAsReal", "3.14", true, 3.14},
};

// Bool overload is strict — only "true" / "false" accepted.
static const ParseCase<bool> scBoolCases[] = {
	{"LiteralTrue_Succeeds",      "true",    true,  true},
	{"LiteralFalse_Succeeds",     "false",   true,  false},
	{"NotABool_FailsStrict",      "garbage", false, false},
	{"EmptyString_FailsStrict",   "",        false, false},
	{"NumericOne_FailsStrict",    "1",       false, false},
	{"MixedCaseTrue_FailsStrict", "True",    false, false},
};

// ---- TryParseOrDefault<T> ------------------------------------------------

template <typename T>
struct ParseOrDefaultCase {
	const char* label;
	const char* input;
	T           defaultValue;
	T           expectedFinal;
};

template <typename T>
static bool RunParseOrDefaultCases(const char* inTypeLabel,
                                   const ParseOrDefaultCase<T>* inCases,
                                   size_t inCount) {
	for(size_t i = 0; i < inCount; ++i) {
		const ParseOrDefaultCase<T>& testCase = inCases[i];

		// Arrange
		T outValue = T();

		// Act
		TryParseOrDefault(string(testCase.input), outValue, testCase.defaultValue);

		// Assert
		if(outValue != testCase.expectedFinal) {
			cout << "SafeParseTest [" << inTypeLabel << "::" << testCase.label
			     << "]: outValue " << outValue
			     << ", expected " << testCase.expectedFinal << endl;
			return false;
		}
	}
	return true;
}

static const ParseOrDefaultCase<int> scOrDefaultIntCases[] = {
	{"ParseSucceeds_WritesParsedValue", "5",   99, 5},
	{"ParseFails_WritesDefault",        "ABC", 99, 99},
	{"EmptyString_WritesDefault",       "",    -1, -1},
};

// ---- wstring overload sanity --------------------------------------------

static bool WStringOverload_ParsesValidInput() {
	// Arrange / Act / Assert
	int parsed = 0;
	if(!TryParse(wstring(L"77"), parsed) || parsed != 77) {
		cout << "SafeParseTest [WString::ParsesValidInput]: "
		        "TryParse(L\"77\") yielded " << parsed << ", expected 77" << endl;
		return false;
	}
	return true;
}

static bool WStringOverload_RejectsNonNumeric() {
	// Arrange / Act / Assert
	int outValue = 0;
	if(TryParse(wstring(L"ABC"), outValue)) {
		cout << "SafeParseTest [WString::RejectsNonNumeric]: "
		        "TryParse(L\"ABC\") returned true, expected false" << endl;
		return false;
	}
	return true;
}

#define COUNT_OF(arr) (sizeof(arr) / sizeof((arr)[0]))

int SafeParseTest(int argc, char* argv[]) {
	(void) argc;
	(void) argv;
	if(!RunParseCases<int>   ("Int",    scIntCases,    COUNT_OF(scIntCases)))    return 1;
	if(!RunParseCases<double>("Double", scDoubleCases, COUNT_OF(scDoubleCases))) return 1;
	if(!RunParseCases<bool>  ("Bool",   scBoolCases,   COUNT_OF(scBoolCases)))   return 1;
	if(!RunParseOrDefaultCases<int>("OrDefaultInt", scOrDefaultIntCases, COUNT_OF(scOrDefaultIntCases))) return 1;
	if(!WStringOverload_ParsesValidInput())   return 1;
	if(!WStringOverload_RejectsNonNumeric())  return 1;
	return 0;
}
