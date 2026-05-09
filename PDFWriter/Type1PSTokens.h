/*
   Source File : Type1PSTokens.h


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
#include <string>

// Stateless PostScript token-string utilities used by the Type 1 (.PFB)
// parser.
class Type1PSTokens
{
public:
	static bool IsComment(const std::string& inToken);
	static std::string FromPSName(const std::string& inPostScriptName);
	static std::string FromPSString(const std::string& inPSString);
};
