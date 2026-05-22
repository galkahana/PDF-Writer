/*
   Source File : SafeParse.h


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
#pragma once

/*
	SafeParse provides parse helpers that distinguish parse failure from a
	successful parse of a value. The historical convenience path through
	BoxingBaseWithRW(const std::string&) silently produces an indeterminate
	(pre-C++11) or deterministic-zero (C++11+) value when extraction fails,
	indistinguishable from a real parse of 0. Prefer these helpers for any
	parse of attacker-controlled input.

	TryParse: returns whether the parse succeeded; outValue is untouched on failure.
	TryParseOrDefault: fire-and-forget; writes inDefault when parse fails.

	Lenient on trailing input (e.g. TryParse("12abc", x) succeeds with x=12) —
	caller-side range/semantic checks are responsible for rejecting wrong
	numbers; this helper only distinguishes "parsed nothing" from "parsed something".
*/

#include "Trace.h"

#include <sstream>
#include <string>

namespace PDFHummus
{

template <typename T>
bool TryParse(const std::string& inReadFrom, T& outValue)
{
	// Note: we deliberately do NOT check stream.eof() after extraction —
	// trailing non-numeric input is accepted (e.g. "12abc" parses to 12).
	// Callers handle wrong/malicious numbers via range checks; this helper
	// only distinguishes "parsed nothing" from "parsed something".
	std::stringstream stream(inReadFrom);
	T tmp;
	stream >> tmp;
	if(stream.fail())
	{
		TRACE_LOG1("SafeParse::TryParse failed for input '%s'", inReadFrom.c_str());
		return false;
	}
	outValue = tmp;
	return true;
}

template <typename T>
bool TryParse(const std::wstring& inReadFrom, T& outValue)
{
	// See lenient-on-trailing-input note on the std::string overload.
	std::wstringstream stream(inReadFrom);
	T tmp;
	stream >> tmp;
	if(stream.fail())
	{
		TRACE_LOG("SafeParse::TryParse<wstring> failed");
		return false;
	}
	outValue = tmp;
	return true;
}

inline bool TryParse(const std::string& inReadFrom, bool& outValue)
{
	if(inReadFrom == "true")  { outValue = true;  return true; }
	if(inReadFrom == "false") { outValue = false; return true; }
	TRACE_LOG1("SafeParse::TryParse<bool> failed for input '%s' (expected \"true\" or \"false\")", inReadFrom.c_str());
	return false;
}

inline bool TryParse(const std::wstring& inReadFrom, bool& outValue)
{
	if(inReadFrom == L"true")  { outValue = true;  return true; }
	if(inReadFrom == L"false") { outValue = false; return true; }
	TRACE_LOG("SafeParse::TryParse<bool,wstring> failed (expected \"true\" or \"false\")");
	return false;
}

template <typename T>
void TryParseOrDefault(const std::string& inReadFrom, T& outValue, const T& inDefault)
{
	if(!TryParse(inReadFrom, outValue))
		outValue = inDefault;
}

template <typename T>
void TryParseOrDefault(const std::wstring& inReadFrom, T& outValue, const T& inDefault)
{
	if(!TryParse(inReadFrom, outValue))
		outValue = inDefault;
}

}  // namespace PDFHummus
