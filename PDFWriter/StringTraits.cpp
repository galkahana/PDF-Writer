/*
   Source File : StringTraits.cpp


   Copyright 2011 Gal Kahana PDFWriter

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
#include "StringTraits.h"

#include <sstream>

StringTraits::StringTraits(const string& inString):mString(inString)
{
}

StringTraits::~StringTraits(void)
{
}

wstring StringTraits::WidenString()
{
	wstringstream formatter;
	string::const_iterator it=mString.begin();
	wchar_t aWideChar;

	for(;it != mString.end();++it)
	{
		aWideChar = (wchar_t)*it;
		formatter.put(aWideChar);
	}
	return formatter.str();
	
}

StringTraits::operator const string&()
{
	return mString;
}
