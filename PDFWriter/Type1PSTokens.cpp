/*
   Source File : Type1PSTokens.cpp


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
#include "Type1PSTokens.h"
#include "IOBasicTypes.h"

#include <sstream>

using namespace IOBasicTypes;

static const std::string scEmpty;

bool Type1PSTokens::IsComment(const std::string& inToken)
{
	return !inToken.empty() && inToken[0] == '%';
}

std::string Type1PSTokens::FromPSName(const std::string& inPostScriptName)
{
	return inPostScriptName.empty() ? scEmpty : inPostScriptName.substr(1);
}

std::string Type1PSTokens::FromPSString(const std::string& inPSString)
{
	// PostScript string literal must contain at least the opening and closing parens.
	if(inPSString.size() < 2)
		return scEmpty;

	std::stringbuf stringBuffer;
	std::string::const_iterator it = inPSString.begin();
	std::string::const_iterator stop = inPSString.end() - 1; // closing paren position
	++it; // skip first paranthesis

	while(it != stop)
	{
		Byte buffer;
		if(*it == '\\')
		{
			++it;
			if(it == stop) break;
			if('0' <= *it && *it <= '7')
			{
				buffer = (*it - '0') * 64;
				++it;
				if(it == stop) break;
				buffer += (*it - '0') * 8;
				++it;
				if(it == stop) break;
				buffer += (*it - '0');
			}
			else
			{
				switch(*it)
				{
					case 'n':
						buffer = '\n';
						break;
					case 'r':
						buffer = '\r';
						break;
					case 't':
						buffer = '\t';
						break;
					case 'b':
						buffer = '\b';
						break;
					case 'f':
						buffer = '\f';
						break;
					case '\\':
						buffer = '\\';
						break;
					case '(':
						buffer = '(';
						break;
					case ')':
						buffer = ')';
						break;
					default:
						// error!
						buffer = 0;
						break;
				}
			}
		}
		else
		{
			buffer = *it;
		}
		stringBuffer.sputn((const char*)&buffer,1);
		++it;
	}
	return stringBuffer.str();
}
