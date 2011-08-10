/*
   Source File : UnicodeEncoding.cpp


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
#include "UnicodeEncoding.h"
#include "Trace.h"

UnicodeEncoding::UnicodeEncoding(void)
{
}

UnicodeEncoding::~UnicodeEncoding(void)
{
}

EPDFStatusCode UnicodeEncoding::UTF16ToUnicode(const wstring& inText,ULongVector& outUnicodeIndexes)
{
	wstring::const_iterator it = inText.begin();
	EPDFStatusCode status = ePDFSuccess;
	unsigned long unicodeCharacter;

	for(; it != inText.end() && ePDFSuccess == status; ++it)
	{
		if(0xD800 <= *it && *it <= 0xDBFF) 
		{
			// Aha! high surrogate! this means that this character requires 2 w_chars
			unsigned short highSurrogate = *it;
			++it;
			if(it == inText.end() || 0xDC00 > *it || *it > 0xDFFF)
			{
				TRACE_LOG("UnicodeEncoding::UTF16ToUnicode, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}
			unsigned short lowSurrogate = *it;

			unicodeCharacter = 0x10000 + ((highSurrogate - 0xD800) << 5) + (lowSurrogate - 0xDC00);
		}
		else
			unicodeCharacter = *it;
		outUnicodeIndexes.push_back(unicodeCharacter);
	}
	return status;
}

bool UnicodeEncoding::IsSupplementary(unsigned long inUnicodeCharacter)
{
	return inUnicodeCharacter > 0xFFFF;
}

UTF16Encoding UnicodeEncoding::EncodeCharater(unsigned long inUnicodeCharacter)
{
	UTF16Encoding encoding;

	if(inUnicodeCharacter > 0xFFFF)
	{
		unsigned long midValue = inUnicodeCharacter - 0x10000;
		encoding.LowSurrogate = (unsigned short)(midValue & 0x3FF) + 0xDC00;
		encoding.HighSurrogate = (unsigned short)((midValue >> 5) & 0x3FF) +  0xD800;
	}
	else
	{
		encoding.HighSurrogate = 0;
		encoding.LowSurrogate = (unsigned short)(inUnicodeCharacter & 0xFFFF);
	}
	return encoding;
}