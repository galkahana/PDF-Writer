#include "UnicodeString.h"
#include "Trace.h"
#include <sstream>

UnicodeString::UnicodeString(void)
{
}

UnicodeString::~UnicodeString(void)
{
}

UnicodeString::UnicodeString(const UnicodeString& inOtherString)
{
	mUnicodeCharacters = inOtherString.mUnicodeCharacters;
}

UnicodeString::UnicodeString(const ULongList& inOtherList)
{
	mUnicodeCharacters = inOtherList;
}

UnicodeString& UnicodeString::operator =(const UnicodeString& inOtherString)
{
	mUnicodeCharacters = inOtherString.mUnicodeCharacters;
	return *this;
}

UnicodeString& UnicodeString::operator =(const ULongList& inOtherList)
{
	mUnicodeCharacters = inOtherList;
	return *this;
}

bool UnicodeString::operator==(const UnicodeString& inOtherString) const
{
	return mUnicodeCharacters == inOtherString.mUnicodeCharacters;
}


const ULongList& UnicodeString::GetUnicodeList() const
{
	return mUnicodeCharacters;
}

ULongList& UnicodeString::GetUnicodeList()
{
	return mUnicodeCharacters;
}

EPDFStatusCode UnicodeString::FromUTF8(const string& inString)
{
	mUnicodeCharacters.clear();
	string::const_iterator it = inString.begin();
	EPDFStatusCode status = ePDFSuccess;
	unsigned long unicodeCharacter;


	for(; it != inString.end() && ePDFSuccess == status; ++it)
	{
		if((unsigned char)*it <= 0x7F)
		{
			unicodeCharacter = (unsigned char)*it;
		}
		else if(((unsigned char)*it>>5) == 0x6) // 2 bytes encoding
		{
			unicodeCharacter = (unsigned char)*it & 0x1F;
			++it;
			if(it == inString.end() || ((unsigned char)*it>>6 != 0x2))
			{
				status = ePDFFailure;
				break;
			}

			unicodeCharacter = (unicodeCharacter << 6) | ((unsigned char)*it & 0x3F);
		}
		else if(((unsigned char)*it>>4) == 0xE) // 3 bytes encoding
		{
			unicodeCharacter = (unsigned char)*it & 0xF;
			for(int i =0 ; i < 2 && ePDFSuccess == status; ++i)
			{
				++it;
				if(it == inString.end() || ((unsigned char)*it>>6 != 0x2))
				{
					status = ePDFFailure;
					break;
				}
				unicodeCharacter = (unicodeCharacter << 6) | ((unsigned char)*it & 0x3F);
			}
			if(status != ePDFSuccess)
				break;
		}
		else if(((unsigned char)*it>>3) == 0x1E) // 4 bytes encoding
		{
			unicodeCharacter = (unsigned char)*it & 0x7;
			for(int i =0 ; i < 3 && ePDFSuccess == status; ++i)
			{
				++it;
				if(it == inString.end() || ((unsigned char)*it>>6 != 0x2))
				{
					status = ePDFFailure;
					break;
				}
				unicodeCharacter = (unicodeCharacter << 6) | ((unsigned char)*it & 0x3F);
			}
			if(status != ePDFSuccess)
				break;
		}
		else
		{
			status = ePDFFailure;
			break;
		}

		mUnicodeCharacters.push_back(unicodeCharacter);
	}

	return status;
}

EPDFStatusCodeAndString UnicodeString::ToUTF8() const
{
	ULongList::const_iterator it = mUnicodeCharacters.begin();
	EPDFStatusCode status = ePDFSuccess;
	stringstream result;

	for(; it != mUnicodeCharacters.end() && ePDFSuccess == status; ++it)
	{
		// Encode Unicode to UTF8
		if(*it <= 0x7F)
		{
			result.put((unsigned char)*it);
		}
		else if(0x7F < *it && *it <= 0x7FF)
		{
			result.put((unsigned char)((0xC0 | (*it>>6))));
			result.put((unsigned char)(0x80 | (*it & 0x3F)));
		}
		else if(0x7FF < *it && *it <= 0xFFFF)
		{
			result.put((unsigned char)(0xE0 | (*it>>12)));
			result.put((unsigned char)(0x80 | ((*it>>6) & 0x3F)));
			result.put((unsigned char)(0x80 | (*it & 0x3F)));
		}
		else if(0xFFFF < *it && *it <= 0x10FFFF)
		{
			result.put((unsigned char)(0xF0 | (*it>>18)));
			result.put((unsigned char)(0x80 | ((*it>>12) & 0x3F)));
			result.put((unsigned char)(0x80 | ((*it>>6) & 0x3F)));
			result.put((unsigned char)(0x80 | (*it & 0x3F)));
		}
		else
		{
			TRACE_LOG("UnicodeString::ToUTF8, contains unicode characters that cannot be coded into UTF8");
			status = ePDFFailure;
		}
	}
	
	return EPDFStatusCodeAndString(status,result.str());
}

EPDFStatusCode UnicodeString::FromUTF16(const string& inString)
{
	return FromUTF16((const unsigned char*)inString.c_str(),(unsigned long)inString.length());
}

EPDFStatusCode UnicodeString::FromUTF16(const unsigned char* inString, unsigned long inLength)
{
	// Read BOM
	if(inLength < 2)
		return ePDFFailure;

	if(inString[0] == 0xFE && inString[1] == 0xFF)
		return FromUTF16BE(inString+2,inLength-2);
	else if(inString[0] == 0xFF && inString[1] == 0xFE)
		return FromUTF16LE(inString+2,inLength-2);
	else
		return ePDFFailure; // no bom
}

EPDFStatusCode UnicodeString::FromUTF16BE(const string& inString)
{
	return FromUTF16BE((const unsigned char*)inString.c_str(),(unsigned long)inString.length());
}

EPDFStatusCode UnicodeString::FromUTF16BE(const unsigned char* inString, unsigned long inLength)
{
	mUnicodeCharacters.clear();
	EPDFStatusCode status = ePDFSuccess;

	if(inLength % 2 != 0)
	{
		TRACE_LOG("UnicodeString::FromUTF16BE, invalid UTF16 string, has odd numbers of characters");
		return ePDFFailure;
	}

	for(unsigned long i = 0; i < inLength-1 && ePDFSuccess == status; i+=2)
	{
		unsigned short buffer = (((unsigned short)inString[i])<<8) + inString[i+1];

		if(0xD800 <= buffer && buffer <= 0xDBFF) 
		{
			// Aha! high surrogate! this means that this character requires 2 w_chars
			unsigned short highSurrogate = buffer;
			i+=2;
			if(i>=inLength-1)
			{
				TRACE_LOG("UnicodeString::FromUTF16BE, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}

			unsigned short buffer = (((unsigned short)inString[i])<<8) + inString[i+1];
			if(0xDC00 > buffer|| buffer > 0xDFFF)
			{
				TRACE_LOG("UnicodeString::FromUTF16BE, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}

			unsigned short lowSurrogate = buffer;

			mUnicodeCharacters.push_back(0x10000 + ((highSurrogate - 0xD800) << 5) + (lowSurrogate - 0xDC00));
		}
		else
			mUnicodeCharacters.push_back(buffer);		
	}

	return status;
}

EPDFStatusCode UnicodeString::FromUTF16LE(const string& inString)
{
	return FromUTF16LE((const unsigned char*)inString.c_str(),(unsigned long)inString.length());
}


EPDFStatusCode UnicodeString::FromUTF16LE(const unsigned char* inString, unsigned long inLength)
{
	mUnicodeCharacters.clear();
	EPDFStatusCode status = ePDFSuccess;

	if(inLength % 2 != 0)
	{
		TRACE_LOG("UnicodeString::FromUTF16LE, invalid UTF16 string, has odd numbers of characters");
		return ePDFFailure;
	}

	for(unsigned long i = 0; i < inLength-1 && ePDFSuccess == status; i+=2)
	{
		unsigned short buffer = (((unsigned short)inString[i+1])<<8) + inString[i];

		if(0xD800 <= buffer && buffer <= 0xDBFF) 
		{
			// Aha! high surrogate! this means that this character requires 2 w_chars
			unsigned short highSurrogate = buffer;
			i+=2;
			if(i>=inLength-1)
			{
				TRACE_LOG("UnicodeString::FromUTF16LE, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}

			unsigned short buffer = (((unsigned short)inString[i+1])<<8) + inString[i];
			if(0xDC00 > buffer|| buffer > 0xDFFF)
			{
				TRACE_LOG("UnicodeString::FromUTF16LE, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}

			unsigned short lowSurrogate = buffer;

			mUnicodeCharacters.push_back(0x10000 + ((highSurrogate - 0xD800) << 5) + (lowSurrogate - 0xDC00));
		}
		else
			mUnicodeCharacters.push_back(buffer);		
	}

	return status;
}

EPDFStatusCode UnicodeString::FromUTF16UShort(unsigned short* inShorts, unsigned long inLength)
{
	mUnicodeCharacters.clear();
	EPDFStatusCode status = ePDFSuccess;

	for(unsigned long i = 0; i < inLength && ePDFSuccess == status; ++i)
	{
		if(0xD800 <= inShorts[i] && inShorts[i] <= 0xDBFF) 
		{
			// Aha! high surrogate! this means that this character requires 2 w_chars
			++i;
			if(i>=inLength)
			{
				TRACE_LOG("UnicodeString::FromUTF16UShort, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}

			if(0xDC00 > inShorts[i] || inShorts[i] > 0xDFFF)
			{
				TRACE_LOG("UnicodeString::FromUTF16UShort, fault string - high surrogat encountered without a low surrogate");
				status = ePDFFailure;
				break;
			}

			mUnicodeCharacters.push_back(0x10000 + ((inShorts[i-1] - 0xD800) << 5) + (inShorts[i] - 0xDC00));
		}
		else
			mUnicodeCharacters.push_back(inShorts[i]);		
	}

	return status;		
}


EPDFStatusCodeAndString UnicodeString::ToUTF16BE(bool inPrependWithBom) const
{
	ULongList::const_iterator it = mUnicodeCharacters.begin();
	EPDFStatusCode status = ePDFSuccess;
	stringstream result;

	if(inPrependWithBom)
	{
		result.put((unsigned char)0xFE);
		result.put((unsigned char)0xFF);
	}

	for(; it != mUnicodeCharacters.end() && ePDFSuccess == status; ++it)
	{
		if(*it < 0xD7FF || (0xE000 < *it && *it < 0xFFFF))
		{
			result.put((unsigned char)(*it>>8));
			result.put((unsigned char)(*it & 0xFF));
		}
		else if(0xFFFF < *it && *it <= 0x10FFFF)
		{
			unsigned short highSurrogate = (unsigned short)(((*it - 0x10000) >> 10) + 0xD800);
			unsigned short lowSurrogate = (unsigned short)(((*it - 0x10000) & 0x3FF) + 0xDC00);
			
			result.put((unsigned char)(highSurrogate>>8));
			result.put((unsigned char)(highSurrogate & 0xFF));
			result.put((unsigned char)(lowSurrogate>>8));
			result.put((unsigned char)(lowSurrogate & 0xFF));
		}
		else
		{
			status = ePDFFailure;
			break;
		}	
	}
	
	return EPDFStatusCodeAndString(status,result.str());
}

EPDFStatusCodeAndString UnicodeString::ToUTF16LE(bool inPrependWithBom) const
{
	ULongList::const_iterator it = mUnicodeCharacters.begin();
	EPDFStatusCode status = ePDFSuccess;
	stringstream result;

	if(inPrependWithBom)
	{
		result.put((unsigned char)0xFF);
		result.put((unsigned char)0xFE);
	}

	for(; it != mUnicodeCharacters.end() && ePDFSuccess == status; ++it)
	{
		if(*it < 0xD7FF || (0xE000 < *it && *it < 0xFFFF))
		{
			result.put((unsigned char)(*it & 0xFF));
			result.put((unsigned char)(*it>>8));
		}
		else if(0xFFFF < *it && *it <= 0x10FFFF)
		{
			unsigned short highSurrogate = (unsigned short)(((*it - 0x10000) >> 10) + 0xD800);
			unsigned short lowSurrogate = (unsigned short)(((*it - 0x10000) & 0x3FF) + 0xDC00);
			
			result.put((unsigned char)(highSurrogate & 0xFF));
			result.put((unsigned char)(highSurrogate>>8));
			result.put((unsigned char)(lowSurrogate & 0xFF));
			result.put((unsigned char)(lowSurrogate>>8));
		}
		else
		{
			status = ePDFFailure;
			break;
		}	
	}
	
	return EPDFStatusCodeAndString(status,result.str());
}

EPDFStatusCodeAndUShortList UnicodeString::ToUTF16UShort() const
{
	ULongList::const_iterator it = mUnicodeCharacters.begin();
	EPDFStatusCode status = ePDFSuccess;
	UShortList result;

	for(; it != mUnicodeCharacters.end() && ePDFSuccess == status; ++it)
	{
		if(*it < 0xD7FF || (0xE000 < *it && *it < 0xFFFF))
		{
			result.push_back((unsigned short)*it);
		}
		else if(0xFFFF < *it && *it <= 0x10FFFF)
		{
			unsigned short highSurrogate = (unsigned short)(((*it - 0x10000) >> 10) + 0xD800);
			unsigned short lowSurrogate = (unsigned short)(((*it - 0x10000) & 0x3FF) + 0xDC00);

			result.push_back(highSurrogate);
			result.push_back(lowSurrogate);
		}
		else
		{
			status = ePDFFailure;
			break;
		}	
	}
	
	return EPDFStatusCodeAndUShortList(status,result);
}


