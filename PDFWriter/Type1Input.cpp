/*
   Source File : Type1Input.cpp


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
#include "Type1Input.h"
#include "IByteReaderWithPosition.h"
#include "SafeParse.h"
#include "StandardEncoding.h"
#include "Trace.h"
#include "CharStringType1Interpreter.h"
#include "Type1PSTokens.h"

#include <algorithm>

// Recursion depth cap for the Type 1 seac dependency walk in
// CollectComponentGlyphs. Adobe's Type 1 Font Format specification
// disallows nested seac (bchar/achar must not be seac characters), so
// 16 is well beyond any conforming font but matches the TrueType
// composite cap used elsewhere in the codebase. Lenient on malformed-
// but-benign fonts; still bounds the call stack against attacker input.
static const unsigned int scMaxCompositeDepth = 16;

using namespace PDFHummus;

// Conservative caps on attacker-controlled Type 1 sizes.
// Real fonts are far below these; a malformed font with values above
// is rejected rather than driving allocations / array indexing OOB.
#define MAX_TYPE1_SUBRS_COUNT 65535
#define MAX_TYPE1_CODE_LENGTH 65535

// Parse a token as a long and verify it falls within [inMinInclusive, inMaxInclusive].
// Used to bound attacker-controlled counts/indices/lengths from PFB tokens before
// they drive allocations or array indexing.
static bool TryParseBoundedLong(const std::string& inToken,long inMinInclusive,long inMaxInclusive,long& outValue)
{
	if(!TryParse(inToken, outValue))
		return false;
	return outValue >= inMinInclusive && outValue <= inMaxInclusive;
}

Type1Input::Type1Input(void)
{
	mSubrsCount = 0;
	mSubrs = NULL;
	mCurrentDependencies = NULL;
}

Type1Input::~Type1Input(void)
{
	FreeTables();
}

void Type1Input::FreeSubrs()
{
	for(long i=0;i<mSubrsCount;++i)
		delete[] mSubrs[i].Code;
	delete[] mSubrs;
	mSubrs = NULL;
	mSubrsCount = 0;
}

void Type1Input::FreeCharStrings()
{
	StringToType1CharStringMap::iterator itCharStrings = mCharStrings.begin();

	for(; itCharStrings != mCharStrings.end(); ++itCharStrings)
		delete[] itCharStrings->second.Code;
	mCharStrings.clear();
}

void Type1Input::FreeTables()
{
	FreeSubrs();
	FreeCharStrings();
}

void Type1Input::Reset()
{
	// Reset type 1 data to default values

	FreeTables();

	mFontDictionary.FontMatrix[1] = mFontDictionary.FontMatrix[2] = mFontDictionary.FontMatrix[4] = mFontDictionary.FontMatrix[5] = 0;
	mFontDictionary.FontMatrix[0] = mFontDictionary.FontMatrix[3] = 0.001;
	mFontDictionary.UniqueID = -1;
	for(int i=0;i<256;++i)
		mEncoding.mCustomEncoding[i].clear();
	mReverseEncoding.clear();
	mFontDictionary.StrokeWidth = 1;
	mFontDictionary.PaintType = 0;
	mFontDictionary.FontType = 1;
	mFontDictionary.FontBBox[0] = mFontDictionary.FontBBox[1] = mFontDictionary.FontBBox[2] = mFontDictionary.FontBBox[3] = 0;
	mFontDictionary.FSTypeValid = false;
	mFontDictionary.fsType = 0;

	mFontInfoDictionary.isFixedPitch = false;
	mFontInfoDictionary.ItalicAngle = 0;
	mFontInfoDictionary.UnderlinePosition = 0;
	mFontInfoDictionary.UnderlineThickness = 0;
	mFontInfoDictionary.Notice.clear();
	mFontInfoDictionary.version.clear();
	mFontInfoDictionary.Weight.clear();
	mFontInfoDictionary.Copyright.clear();
	mFontInfoDictionary.FSTypeValid = false;
	mFontInfoDictionary.fsType = 0;

	mPrivateDictionary.BlueFuzz = 1;
	mPrivateDictionary.BlueScale = 0.039625;
	mPrivateDictionary.BlueShift = 7;
	mPrivateDictionary.BlueValues.clear();
	mPrivateDictionary.OtherBlues.clear();
	mPrivateDictionary.FamilyBlues.clear();
	mPrivateDictionary.FamilyOtherBlues.clear();
	mPrivateDictionary.ForceBold = false;
	mPrivateDictionary.LanguageGroup = 0;
	mPrivateDictionary.lenIV = 4;
	mPrivateDictionary.RndStemUp = false;
	mPrivateDictionary.StdHW = -1;
	mPrivateDictionary.StdVW = -1;
	mPrivateDictionary.StemSnapH.clear();
	mPrivateDictionary.StemSnapV.clear();
	mPrivateDictionary.UniqueID = -1;
}

EStatusCode Type1Input::ReadType1File(IByteReaderWithPosition* inType1)
{
	EStatusCode status = eSuccess;
	BoolAndString token;


	Reset();

	do
	{
		status = mPFBDecoder.Assign(inType1);
		if(status != eSuccess)
			break;

		// the fun about pfb decoding is that it's pretty much token based...so let's do some tokening
		while(mPFBDecoder.NotEnded() && eSuccess == status)
		{
			token = mPFBDecoder.GetNextToken();
			status = mPFBDecoder.GetInternalState();
			// token not having been read, is not necessarily bad. could be a result of segment ending. 
			// so to get whether there's an actual failure, i'm using the internal state.
			if(!token.first) 
				continue;

			// skip comments
			if(Type1PSTokens::IsComment(token.second))
				continue;

			// look for "begin". at this level that would be catching the "begin"
			// of the font dictionary
			if(token.second.compare("begin") == 0)
			{
				status = ReadFontDictionary();
				if(status != eSuccess)
					break;
			}

			// parsing private. note that while charstrings are not defined under private,
			// they are defined before its "end". hence parsing for them will take place at the
			// private dictionary parsing.
			if(token.second.compare("/Private") == 0)
			{
				status = ReadPrivateDictionary();
				if(status != eSuccess)
					break;
			}
			
		}

		/*
		charstring/subrs
		*/

	}while(false);

	mPFBDecoder.Assign(NULL);
	return status;
}

bool Type1Input::ReadNextTokenValue(std::string& outValue,EStatusCode& outStatus)
{
	BoolAndString token = mPFBDecoder.GetNextToken();
	outValue = token.second;
	outStatus = token.first ? eSuccess : eFailure;
	return token.first;
}

EStatusCode Type1Input::ReadFontDictionary()
{
	EStatusCode status = eSuccess;
	BoolAndString token;
	std::string value;

	while(mPFBDecoder.NotEnded() && eSuccess == status)
	{
		token = mPFBDecoder.GetNextToken();
		status = mPFBDecoder.GetInternalState();
		if(!token.first) 
			continue;

		// skip comments
		if(Type1PSTokens::IsComment(token.second))
			continue;

		// found end, done with dictionary
		if(token.second.compare("end") == 0)
			break;

		if(token.second.compare("/FontInfo") == 0)
		{
			status = ReadFontInfoDictionary();
			continue;
		}
		if(token.second.compare("/FontName") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontDictionary.FontName = Type1PSTokens::FromPSName(value);
			continue;
		}
		if(token.second.compare("/PaintType") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontDictionary.PaintType))
			{
				TRACE_LOG1("Type1Input::ReadFontDictionary, /PaintType has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/FontType") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontDictionary.FontType))
			{
				TRACE_LOG1("Type1Input::ReadFontDictionary, /FontType has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/FontMatrix") == 0)
		{
			status = ParseDoubleArray(mFontDictionary.FontMatrix,6);
			continue;
		}

		if(token.second.compare("/FontBBox") == 0)
		{
			status = ParseDoubleArray(mFontDictionary.FontBBox,4);
			continue;
		}

		if(token.second.compare("/UniqueID") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontDictionary.UniqueID))
			{
				TRACE_LOG1("Type1Input::ReadFontDictionary, /UniqueID has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}

		if(token.second.compare("/StrokeWidth") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontDictionary.StrokeWidth))
			{
				TRACE_LOG1("Type1Input::ReadFontDictionary, /StrokeWidth has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}

		if(token.second.compare("/Encoding") == 0)
		{
			status = ParseEncoding();
			if(eSuccess == status)
				CalculateReverseEncoding();
			continue;
		}

		if(token.second.compare("/FSType") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontInfoDictionary.fsType))
			{
				TRACE_LOG1("Type1Input::ReadFontDictionary, /FSType has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			mFontInfoDictionary.FSTypeValid = true;
		}
	}
	return status;
}

EStatusCode Type1Input::ReadFontInfoDictionary()
{
	EStatusCode status = eSuccess;
	BoolAndString token;
	std::string value;

  // initialize some values to defaults
  mFontInfoDictionary.ItalicAngle = 0.0;
  mFontInfoDictionary.UnderlinePosition = 0.0;
  mFontInfoDictionary.UnderlineThickness = 0.0;
  
	while(mPFBDecoder.NotEnded() && eSuccess == status)
	{
		token = mPFBDecoder.GetNextToken();
		status = mPFBDecoder.GetInternalState();
		if(!token.first) 
			continue;

		// skip comments
		if(Type1PSTokens::IsComment(token.second))
			continue;

		// "end" encountered, dictionary finished, return.
		if(token.second.compare("end") == 0) 
			break;

		if(token.second.compare("/version") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontInfoDictionary.version = Type1PSTokens::FromPSString(value);
			continue;
		}
		if(token.second.compare("/Notice") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontInfoDictionary.Notice = Type1PSTokens::FromPSString(value);
			continue;
		}
		if(token.second.compare("/Copyright") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontInfoDictionary.Copyright = Type1PSTokens::FromPSString(value);
			continue;
		}
		if(token.second.compare("/FullName") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontInfoDictionary.FullName = Type1PSTokens::FromPSString(value);
			continue;
		}
		if(token.second.compare("/FamilyName") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontInfoDictionary.FamilyName = Type1PSTokens::FromPSString(value);
			continue;
		}
		if(token.second.compare("/Weight") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			mFontInfoDictionary.Weight = Type1PSTokens::FromPSString(value);
			continue;
		}
		if(token.second.compare("/ItalicAngle") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontInfoDictionary.ItalicAngle))
			{
				TRACE_LOG1("Type1Input::ReadFontInfoDictionary, /ItalicAngle has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/isFixedPitch") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontInfoDictionary.isFixedPitch))
			{
				TRACE_LOG1("Type1Input::ReadFontInfoDictionary, /isFixedPitch has bad boolean value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/UnderlinePosition") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontInfoDictionary.UnderlinePosition))
			{
				TRACE_LOG1("Type1Input::ReadFontInfoDictionary, /UnderlinePosition has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/UnderlineThickness") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontInfoDictionary.UnderlineThickness))
			{
				TRACE_LOG1("Type1Input::ReadFontInfoDictionary, /UnderlineThickness has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}

		if(token.second.compare("/FSType") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mFontInfoDictionary.fsType))
			{
				TRACE_LOG1("Type1Input::ReadFontInfoDictionary, /FSType has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			mFontInfoDictionary.FSTypeValid = true;
		}
	}
	return status;
}

EStatusCode Type1Input::ParseDoubleArray(double* inArray,int inArraySize)
{
	EStatusCode status = eSuccess;

	// skip the [ or {
	BoolAndString token = mPFBDecoder.GetNextToken();
	if(!token.first)
		return eFailure;

	for(int i=0; i < inArraySize && eSuccess == status;++i)
	{
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
		{
			status = eFailure;
			break;
		}
		if(!TryParse(token.second, inArray[i]))
		{
			TRACE_LOG1("Type1Input::ParseDoubleArray, bad numeric value '%s'", token.second.c_str());
			status = eFailure;
			break;
		}
	}

	// skip the last ] or }
	token = mPFBDecoder.GetNextToken();
	if(!token.first)
		return eFailure;

	return status;

}

EStatusCode Type1Input::ParseEncoding()
{
	BoolAndString token = mPFBDecoder.GetNextToken();
	EStatusCode status = eSuccess;
	int encodingIndex = 0;

	if(!token.first)
		return eFailure;

	// checking for standard encoding
	if(token.second.compare("StandardEncoding") == 0)
	{
		mEncoding.EncodingType = eType1EncodingTypeStandardEncoding;

		// skip the def
		BoolAndString token = mPFBDecoder.GetNextToken();
		if(!token.first)
			return eFailure;
		return eSuccess;
	}

	// not standard encoding, parse custom encoding
	mEncoding.EncodingType = eType1EncodingTypeCustom;

	// hop hop...skip to the first dup
	while(token.first)
	{
		token = mPFBDecoder.GetNextToken();
		if(token.second.compare("dup") == 0)
			break;
	}
	if(!token.first)
		return eFailure;

	// k. now parse the repeats of "dup index charactername put"
	// till the first occurence of "readonly" or "def".
	while(token.first)
	{
		if(token.second.compare("readonly") == 0 || token.second.compare("def") == 0)
			break;

		// get the index from the next token
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
			break;
		if(!TryParse(token.second, encodingIndex))
		{
			TRACE_LOG1("Type1Input::ParseEncoding, encoding index has bad numeric value '%s'", token.second.c_str());
			status = eFailure;
			break;
		}
		if(encodingIndex < 0 || encodingIndex > 255)
		{
			status = eFailure;
			break;
		}
		
		// get the glyph name
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
			break;
		mEncoding.mCustomEncoding[encodingIndex] = Type1PSTokens::FromPSName(token.second);

		// skip the put
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
			break;

		// get next row first token [dup or end]
		token = mPFBDecoder.GetNextToken();
	}
	if(!token.first || status != eSuccess)
		return eFailure;

	return status;
}

bool Type1Input::IsCustomEncoding()
{
    return eType1EncodingTypeCustom == mEncoding.EncodingType;
}

void Type1Input::CalculateReverseEncoding()
{
	StringToByteMap::iterator it;

	mReverseEncoding.insert(StringToByteMap::value_type("",0));
	mReverseEncoding.insert(StringToByteMap::value_type(".notdef",0));
	if(eType1EncodingTypeCustom == mEncoding.EncodingType)
	{
		for(int i=0; i <256;++i)
		{
			it = mReverseEncoding.find(mEncoding.mCustomEncoding[i]);
			if(it == mReverseEncoding.end())
				mReverseEncoding.insert(StringToByteMap::value_type(mEncoding.mCustomEncoding[i],i));
			
		}
	}
	else
	{
		StandardEncoding standardEncoding;

		for(int i=0; i <256;++i)
		{
			it = mReverseEncoding.find(standardEncoding.GetEncodedGlyphName(i));
			if(it == mReverseEncoding.end())
				mReverseEncoding.insert(StringToByteMap::value_type(standardEncoding.GetEncodedGlyphName(i),i));
			
		}
	}
}

EStatusCode Type1Input::ReadPrivateDictionary()
{

	EStatusCode status = eSuccess;
    bool readCharString = false; // don't leave before you read CharStrings. so i'm having a little flag
	BoolAndString token;
	std::string value;

	while(mPFBDecoder.NotEnded() && eSuccess == status)
	{
		token = mPFBDecoder.GetNextToken();
		status = mPFBDecoder.GetInternalState();
		if(!token.first) 
			continue;

		// skip comments
		if(Type1PSTokens::IsComment(token.second))
			continue;

		// "end" encountered, dictionary finished, return.
		if(token.second.compare("end") == 0 && readCharString)
			break;

		if(token.second.compare("/UniqueID") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.UniqueID))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /UniqueID has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}

		if(token.second.compare("/BlueValues") == 0)
		{
			status = ParseIntVector(mPrivateDictionary.BlueValues);
			continue;
		}
		if(token.second.compare("/OtherBlues") == 0)
		{
			status = ParseIntVector(mPrivateDictionary.OtherBlues);
			continue;
		}
		if(token.second.compare("/FamilyBlues") == 0)
		{
			status = ParseIntVector(mPrivateDictionary.FamilyBlues);
			continue;
		}
		if(token.second.compare("/FamilyOtherBlues") == 0)
		{
			status = ParseIntVector(mPrivateDictionary.FamilyOtherBlues);
			continue;
		}
		if(token.second.compare("/BlueScale") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.BlueScale))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /BlueScale has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/BlueShift") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.BlueShift))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /BlueShift has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/BlueFuzz") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.BlueFuzz))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /BlueFuzz has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/StdHW") == 0)
		{
			if(!ReadNextTokenValue(value,status)) // skip [
				break;
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.StdHW))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /StdHW has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			if(!ReadNextTokenValue(value,status)) // skip ]
				break;
			continue;
		}
		if(token.second.compare("/StdVW") == 0)
		{
			if(!ReadNextTokenValue(value,status)) // skip [
				break;
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.StdVW))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /StdVW has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			if(!ReadNextTokenValue(value,status)) // skip ]
				break;
			continue;
		}
		if(token.second.compare("/StemSnapH") == 0)
		{
			status = ParseDoubleVector(mPrivateDictionary.StemSnapH);
			continue;
		}
		if(token.second.compare("/StemSnapV") == 0)
		{
			status = ParseDoubleVector(mPrivateDictionary.StemSnapV);
			continue;
		}
		if(token.second.compare("/ForceBold") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.ForceBold))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /ForceBold has bad boolean value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/LanguageGroup") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.LanguageGroup))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /LanguageGroup has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/lenIV") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.lenIV))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /lenIV has bad numeric value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/RndStemUp") == 0)
		{
			if(!ReadNextTokenValue(value,status))
				break;
			if(!TryParse(value, mPrivateDictionary.RndStemUp))
			{
				TRACE_LOG1("Type1Input::ReadPrivateDictionary, /RndStemUp has bad boolean value '%s'", value.c_str());
				status = eFailure;
				break;
			}
			continue;
		}
		if(token.second.compare("/Subrs") == 0)
		{
			status = ParseSubrs();
			continue;
		}

		if(token.second.compare("/CharStrings") == 0)
		{
			status = ParseCharstrings();
            readCharString = true;
			continue;

		}
	}
	return status;	
}

EStatusCode Type1Input::ParseIntVector(std::vector<int>& inVector)
{
	// skip the [ or {
	BoolAndString token = mPFBDecoder.GetNextToken();
	if(!token.first)
		return eFailure;


	while(token.first)
	{
		token = mPFBDecoder.GetNextToken();
		if(token.second.compare("]") == 0 || token.second.compare("}") == 0)
			break;

		int parsed;
		if(!TryParse(token.second, parsed))
		{
			TRACE_LOG1("Type1Input::ParseIntVector, bad numeric value '%s'", token.second.c_str());
			return eFailure;
		}
		inVector.push_back(parsed);
	}
	return token.first ? eSuccess:eFailure;
}

EStatusCode Type1Input::ParseDoubleVector(std::vector<double>& inVector)
{
	// skip the [ or {
	BoolAndString token = mPFBDecoder.GetNextToken();
	if(!token.first)
		return eFailure;


	while(token.first)
	{
		token = mPFBDecoder.GetNextToken();
		if(token.second.compare("]") == 0 || token.second.compare("}") == 0)
			break;

		double parsed;
		if(!TryParse(token.second, parsed))
		{
			TRACE_LOG1("Type1Input::ParseDoubleVector, bad numeric value '%s'", token.second.c_str());
			return eFailure;
		}
		inVector.push_back(parsed);
	}
	return token.first ? eSuccess:eFailure;
}

EStatusCode Type1Input::ParseSubrs()
{
	EStatusCode status = eSuccess;
	long newCount;
	long subrIndex;
	BoolAndString token;

	// Drop any previously parsed subrs first - handles a malformed font that
	// declares /Subrs more than once and guarantees the failure path below
	// finds a clean slate to leave behind via FreeSubrs().
	FreeSubrs();

	do
	{
		// get the subrs count
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
		{
			status = eFailure;
			break;
		}

		if(!TryParseBoundedLong(token.second,0,MAX_TYPE1_SUBRS_COUNT,newCount))
		{
			TRACE_LOG1("Type1Input::ParseSubrs, subrs count out of range: %ld",newCount);
			status = eFailure;
			break;
		}
		if(newCount == 0)
			break; // status stays eSuccess; no allocation needed

		mSubrsCount = newCount;
		mSubrs = new Type1CharString[mSubrsCount];

		// parse the subrs. they look like this:
		// dup index nbytes RD ~n~binary~bytes~ NP

		// skip till the first dup
		while(token.first)
		{
			token = mPFBDecoder.GetNextToken();
			if(token.second.compare("dup") == 0)
				break;
		}
		if(!token.first)
		{
			status = eFailure;
			break;
		}

		for(long i=0;i<mSubrsCount && token.first && status == eSuccess;++i)
		{
			token = mPFBDecoder.GetNextToken();
			if(!token.first)
			{
				status = eFailure;
				break;
			}

			if(!TryParseBoundedLong(token.second,0,mSubrsCount - 1,subrIndex))
			{
				TRACE_LOG2("Type1Input::ParseSubrs, subr index %ld out of range [0,%ld)",subrIndex,mSubrsCount);
				status = eFailure;
				break;
			}
			token = mPFBDecoder.GetNextToken();
			if(!token.first)
			{
				status = eFailure;
				break;
			}

			long codeLength;
			if(!TryParseBoundedLong(token.second,1,MAX_TYPE1_CODE_LENGTH,codeLength))
			{
				TRACE_LOG1("Type1Input::ParseSubrs, subr CodeLength out of range: %ld",codeLength);
				status = eFailure;
				break;
			}
			// Free any previous Code buffer at this index in case a malformed
			// font defines the same subr twice. delete[] NULL is well-defined
			// (default ctor zero-inits Code) so the first-time path is fine.
			delete[] mSubrs[subrIndex].Code;
			mSubrs[subrIndex].Code = NULL;
			mSubrs[subrIndex].CodeLength = (int)codeLength;
			mSubrs[subrIndex].Code = new Byte[mSubrs[subrIndex].CodeLength];

			// skip the RD token (will also skip space)
			mPFBDecoder.GetNextToken();

			mPFBDecoder.Read(mSubrs[subrIndex].Code,mSubrs[subrIndex].CodeLength);

			// skip till next line or array end
			while ( token.first )
			{
				token = mPFBDecoder.GetNextToken();

				if ( 0 == token.second.compare("dup") )
					break;
				if ( 0 == token.second.compare("ND") )
					break;
				if ( 0 == token.second.compare("|-") ) // synonym for "ND"
					break;
				if ( 0 == token.second.compare("def") )
					break;
			}
		}
		if(status != eSuccess)
			break;

		status = mPFBDecoder.GetInternalState();
	} while(false);

	if(status != eSuccess)
		FreeSubrs(); // leave a clean state on any failure rather than partial subrs

	return status;
}

EStatusCode Type1Input::ParseCharstrings()
{
	EStatusCode status = eSuccess;
	BoolAndString token;
	std::string characterName;
	Type1CharString charString;

	// Drop any previously parsed charstrings first - handles a malformed font
	// that declares /CharStrings more than once and guarantees the failure
	// path below finds a clean slate to leave behind via FreeCharStrings().
	FreeCharStrings();

	do
	{
		// skip till "begin"
		while(mPFBDecoder.NotEnded())
		{
			token = mPFBDecoder.GetNextToken();
			if(!token.first || token.second.compare("begin") == 0)
				break;
		}
		if(!token.first)
		{
			status = eFailure;
			break;
		}

		// Charstrings look like this:
		// charactername nbytes RD ~n~binary~bytes~ ND
		while(token.first && mPFBDecoder.GetInternalState() == eSuccess)
		{
			token = mPFBDecoder.GetNextToken();

			if("end" == token.second)
				break;

			characterName = Type1PSTokens::FromPSName(token.second);

			long codeLength;
			if(!TryParseBoundedLong(mPFBDecoder.GetNextToken().second,1,MAX_TYPE1_CODE_LENGTH,codeLength))
			{
				TRACE_LOG1("Type1Input::ParseCharstrings, charstring CodeLength out of range: %ld",codeLength);
				status = eFailure;
				break;
			}

			charString.CodeLength = (int)codeLength;

			charString.Code = new Byte[charString.CodeLength];

			// skip the RD token (will also skip space)
			mPFBDecoder.GetNextToken();

			// Reject a short read: the unwritten tail of Code would
			// otherwise reach the charstring interpreter as indeterminate data.
			if(mPFBDecoder.Read(charString.Code,charString.CodeLength) != (LongBufferSizeType)charString.CodeLength)
			{
				TRACE_LOG1("Type1Input::ParseCharstrings, truncated charstring data for %s",characterName.c_str());
				delete[] charString.Code;
				charString.Code = NULL;
				status = eFailure;
				break;
			}

			// std::map::insert is no-op on duplicate keys; without checking we
			// would leak charString.Code on a malformed font that names the
			// same charstring twice. Reject the duplicate cleanly so the
			// outer FreeCharStrings cleanup runs on the prior entries.
			std::pair<StringToType1CharStringMap::iterator,bool> insertResult =
				mCharStrings.insert(StringToType1CharStringMap::value_type(characterName,charString));
			if(!insertResult.second)
			{
				TRACE_LOG1("Type1Input::ParseCharstrings, duplicate charstring name: %s",characterName.c_str());
				delete[] charString.Code;
				charString.Code = NULL;
				status = eFailure;
				break;
			}

			// skip ND token
			mPFBDecoder.GetNextToken();
		}
		if(status != eSuccess)
			break;

		status = mPFBDecoder.GetInternalState();
	} while(false);

	if(status != eSuccess)
		FreeCharStrings(); // leave a clean state on any failure rather than partial charstrings

	return status;
}

Type1CharString* Type1Input::GetGlyphCharString(Byte inCharStringIndex)
{
	std::string characterName;

	if(eType1EncodingTypeCustom == mEncoding.EncodingType)
	{
		if(mEncoding.mCustomEncoding[inCharStringIndex].size() == 0)
			characterName = ".notdef";
		else
			characterName = mEncoding.mCustomEncoding[inCharStringIndex];
			
	}
	else
	{
		StandardEncoding standardEncoding;
		
		characterName = standardEncoding.GetEncodedGlyphName(inCharStringIndex);
	}
	
	StringToType1CharStringMap::iterator it = mCharStrings.find(characterName);
	if(it == mCharStrings.end())
		return NULL;
	else
		return &(it->second);
}

Type1CharString* Type1Input::GetGlyphCharString(const std::string& inCharStringName)
{
	StringToType1CharStringMap::iterator it = mCharStrings.find(inCharStringName);
	if(it == mCharStrings.end())
		return NULL;
	else
		return &(it->second);
}


EStatusCode Type1Input::CalculateDependenciesForCharIndex(	Byte inCharStringIndex,
															CharString1Dependencies& ioDependenciesInfo)
{
	CharStringType1Interpreter interpreter;

	Type1CharString* charString = GetGlyphCharString(inCharStringIndex);
	if(!charString)
	{
		TRACE_LOG("Type1Input::CalculateDependenciesForCharIndex, Exception, cannot find glyph index");
		return eFailure;
	}

	mCurrentDependencies = &ioDependenciesInfo;
	EStatusCode status = interpreter.Intepret(*charString,this);
	mCurrentDependencies = NULL;
	return status;
}

EStatusCode Type1Input::CalculateDependenciesForCharIndex(const std::string& inCharStringName,
											  CharString1Dependencies& ioDependenciesInfo)
{
	CharStringType1Interpreter interpreter;
	StringToType1CharStringMap::iterator it = mCharStrings.find(inCharStringName);

	if(it == mCharStrings.end())
	{
		TRACE_LOG("Type1Input::CalculateDependenciesForCharIndex, Exception, cannot find glyph from name");
		return eFailure;
	}

	mCurrentDependencies = &ioDependenciesInfo;
	EStatusCode status = interpreter.Intepret(it->second,this);
	mCurrentDependencies = NULL;
	return status;
}

EStatusCode Type1Input::AddDependentGlyphs(StringVector& ioSubsetGlyphIDs)
{
	EStatusCode status = PDFHummus::eSuccess;
	StringSet glyphsSet;
	StringVector::iterator it = ioSubsetGlyphIDs.begin();
	bool hasCompositeGlyphs = false;

	for(; it != ioSubsetGlyphIDs.end() && PDFHummus::eSuccess == status; ++it)
	{
		bool localHasCompositeGlyphs;
		status = CollectComponentGlyphs(*it, glyphsSet, localHasCompositeGlyphs);
		hasCompositeGlyphs |= localHasCompositeGlyphs;
	}

	if(hasCompositeGlyphs)
	{
		for(it = ioSubsetGlyphIDs.begin(); it != ioSubsetGlyphIDs.end(); ++it)
			glyphsSet.insert(*it);

		ioSubsetGlyphIDs.clear();
		for(StringSet::iterator itNewGlyphs = glyphsSet.begin(); itNewGlyphs != glyphsSet.end(); ++itNewGlyphs)
			ioSubsetGlyphIDs.push_back(*itNewGlyphs);

		std::sort(ioSubsetGlyphIDs.begin(), ioSubsetGlyphIDs.end());
	}
	return status;
}

EStatusCode Type1Input::CollectComponentGlyphs(const std::string& inGlyphID,
											   StringSet& ioComponents,
											   bool& outFoundComponents,
											   unsigned int inDepth)
{
	outFoundComponents = false;

	if(inDepth > scMaxCompositeDepth)
	{
		// Cycles are blocked by the visited-set guard below, but a malicious
		// font can still build a deeply nested acyclic seac chain. Cap depth
		// to keep the call stack bounded.
		TRACE_LOG2("Type1Input::CollectComponentGlyphs, composite depth %u exceeds cap %u, refusing to recurse further.",
			inDepth, scMaxCompositeDepth);
		return PDFHummus::eSuccess;
	}

	CharString1Dependencies dependencies;
	StandardEncoding standardEncoding;
	EStatusCode status = CalculateDependenciesForCharIndex(inGlyphID, dependencies);

	if(PDFHummus::eSuccess == status && dependencies.mCharCodes.size() != 0)
	{
		ByteSet::iterator it = dependencies.mCharCodes.begin();
		for(; it != dependencies.mCharCodes.end() && PDFHummus::eSuccess == status; ++it)
		{
			bool dummyFound;
			// Using standard encoding instead of the font encoding, because
			// SEAC (the only operator to create glyph dependency in Type 1)
			// relies on standard encoding indexes by definition.
			std::string glyphName = standardEncoding.GetEncodedGlyphName(*it);
			// Recurse only when the component is new to the set. A glyph
			// referencing itself or two glyphs referencing each other would
			// otherwise drive the call stack until it overflows. The set
			// doubles as the visited marker for cycle detection.
			if(ioComponents.insert(glyphName).second)
				status = CollectComponentGlyphs(glyphName, ioComponents, dummyFound, inDepth + 1);
		}
		outFoundComponents = true;
	}
	return status;
}


Type1CharString* Type1Input::GetSubr(long inSubrIndex)
{
	if(inSubrIndex < 0 || inSubrIndex >= mSubrsCount || !mSubrs)
	{
		TRACE_LOG2("CharStringType1Tracer::GetLocalSubr exception, asked for %ld and there are only %ld count subrs",inSubrIndex,mSubrsCount);
		return NULL;
	}

	if(mCurrentDependencies)
		mCurrentDependencies->mSubrs.insert((unsigned short)inSubrIndex);

	return mSubrs+inSubrIndex;
}

EStatusCode Type1Input::Type1Seac(const LongList& inOperandList)
{
	if(inOperandList.size() < 2) {
		TRACE_LOG1("Type1Input::Type1Seac exception, there should be 2 parameters provided for seac operation but only %d provided",inOperandList.size());
		return eFailure;		
	}

	if(mCurrentDependencies)
	{
		LongList::const_reverse_iterator it = inOperandList.rbegin();

		mCurrentDependencies->mCharCodes.insert((Byte)*it);
		++it;
		mCurrentDependencies->mCharCodes.insert((Byte)*it);
	}
	return eSuccess;
}

bool Type1Input::IsOtherSubrSupported(long inOtherSubrsIndex)
{
	if(mCurrentDependencies)
		mCurrentDependencies->mOtherSubrs.insert((unsigned short)inOtherSubrsIndex);
	return false;
}

unsigned long Type1Input::GetLenIV()
{
	return mPrivateDictionary.lenIV;
}

bool Type1Input::IsValidGlyphIndex(Byte inCharStringIndex)
{
	std::string characterName;

	if(eType1EncodingTypeCustom == mEncoding.EncodingType)
	{
		if(mEncoding.mCustomEncoding[inCharStringIndex].size() == 0)
			characterName = ".notdef";
		else
			characterName = mEncoding.mCustomEncoding[inCharStringIndex];
			
	}
	else
	{
		StandardEncoding standardEncoding;
		
		characterName = standardEncoding.GetEncodedGlyphName(inCharStringIndex);
	}
	
	return mCharStrings.find(characterName) != mCharStrings.end();
}

std::string Type1Input::GetGlyphCharStringName(Byte inCharStringIndex)
{
	if(eType1EncodingTypeCustom == mEncoding.EncodingType)
	{
		if(mEncoding.mCustomEncoding[inCharStringIndex].size() == 0)
			return ".notdef";
		else
			return mEncoding.mCustomEncoding[inCharStringIndex];
	}
	else
	{
		StandardEncoding standardEncoding;

		return standardEncoding.GetEncodedGlyphName(inCharStringIndex);
	}
}

Byte Type1Input::GetEncoding(const std::string& inCharStringName)
{	
	StringToByteMap::iterator it = mReverseEncoding.find(inCharStringName);
	if(it == mReverseEncoding.end())
		return 0;
	else
		return it->second;
}