#include "Type1Input.h"
#include "IByteReaderWithPosition.h"
#include "BoxingBase.h"
#include "PSBool.h"
#include "StandardEncoding.h"
#include "Trace.h"
#include "CharStringType1Interpreter.h"

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

void Type1Input::FreeTables()
{
	for(Byte i=0;i<mSubrsCount;++i)
		delete[] mSubrs[i].Code;
	delete[] mSubrs;
	mSubrs = NULL;
	mSubrsCount = 0;

	StringToType1CharStringMap::iterator itCharStrings = mCharStrings.begin();

	for(; itCharStrings != mCharStrings.end(); ++itCharStrings)
		delete[] itCharStrings->second.Code;
	mCharStrings.clear();
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
	mFontDictionary.StrokeWidth = 1;

	mFontInfoDictionary.isFixedPitch = false;
	mFontInfoDictionary.ItalicAngle = 0;
	mFontInfoDictionary.Notice.clear();
	mFontInfoDictionary.version.clear();
	mFontInfoDictionary.Weight.clear();
	mFontInfoDictionary.Copyright.clear();

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

	Type1PrivateDictionary mPrivateDictionary;
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
			if(IsComment(token.second))
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

bool Type1Input::IsComment(const string& inToken)
{
	return inToken.at(0) == '%';
}

EStatusCode Type1Input::ReadFontDictionary()
{
	EStatusCode status = eSuccess;
	BoolAndString token;

	while(mPFBDecoder.NotEnded() && eSuccess == status)
	{
		token = mPFBDecoder.GetNextToken();
		status = mPFBDecoder.GetInternalState();
		if(!token.first) 
			continue;

		// skip comments
		if(IsComment(token.second))
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
			mFontDictionary.FontName = FromPSName(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/PaintType") == 0)
		{
			mFontDictionary.PaintType = Int(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/FontType") == 0)
		{
			mFontDictionary.FontType = Int(mPFBDecoder.GetNextToken().second);
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
			mFontDictionary.UniqueID = Int(mPFBDecoder.GetNextToken().second);
			continue;
		}

		if(token.second.compare("/StrokeWidth") == 0)
		{
			mFontDictionary.StrokeWidth = Double(mPFBDecoder.GetNextToken().second);
			continue;
		}

		if(token.second.compare("/Encoding") == 0)
		{
			status = ParseEncoding();
			continue;
		}
	}
	return status;
}

EStatusCode Type1Input::ReadFontInfoDictionary()
{
	EStatusCode status = eSuccess;
	BoolAndString token;

	while(mPFBDecoder.NotEnded() && eSuccess == status)
	{
		token = mPFBDecoder.GetNextToken();
		status = mPFBDecoder.GetInternalState();
		if(!token.first) 
			continue;

		// skip comments
		if(IsComment(token.second))
			continue;

		// "end" encountered, dictionary finished, return.
		if(token.second.compare("end") == 0) 
			break;

		if(token.second.compare("/version") == 0)
		{
			mFontInfoDictionary.version = mPFBDecoder.GetNextToken().second;
			continue;
		}
		if(token.second.compare("/Notice") == 0)
		{
			mFontInfoDictionary.Notice = mPFBDecoder.GetNextToken().second;
			continue;
		}
		if(token.second.compare("/Copyright") == 0)
		{
			mFontInfoDictionary.Copyright = mPFBDecoder.GetNextToken().second;
			continue;
		}
		if(token.second.compare("/FullName") == 0)
		{
			mFontInfoDictionary.FullName = mPFBDecoder.GetNextToken().second;
			continue;
		}
		if(token.second.compare("/FamilyName") == 0)
		{
			mFontInfoDictionary.FamilyName = mPFBDecoder.GetNextToken().second;
			continue;
		}
		if(token.second.compare("/Weight") == 0)
		{
			mFontInfoDictionary.Weight = mPFBDecoder.GetNextToken().second;
			continue;
		}
		if(token.second.compare("/ItalicAngle") == 0)
		{
			mFontInfoDictionary.ItalicAngle = 
				Double(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/isFixedPitch") == 0)
		{
			mFontInfoDictionary.isFixedPitch = 
				PSBool(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/UnderlinePosition") == 0)
		{
			mFontInfoDictionary.UnderlinePosition = 
				Double(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/UnderlineThickness") == 0)
		{
			mFontInfoDictionary.UnderlineThickness = 
				Double(mPFBDecoder.GetNextToken().second);
			continue;
		}
	}
	return status;	
}

string Type1Input::FromPSName(const string& inPostScriptName)
{
	return inPostScriptName.substr(1);
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
		status = token.first ? eSuccess:eFailure;
		inArray[i] = Double(token.second);
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
		encodingIndex = Int(token.second);
		if(encodingIndex < 0 || encodingIndex > 255)
		{
			status = eFailure;
			break;
		}
		
		// get the glyph name
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
			break;
		mEncoding.mCustomEncoding[encodingIndex] = FromPSName(token.second);

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

EStatusCode Type1Input::ReadPrivateDictionary()
{

	EStatusCode status = eSuccess;
	BoolAndString token;

	while(mPFBDecoder.NotEnded() && eSuccess == status)
	{
		token = mPFBDecoder.GetNextToken();
		status = mPFBDecoder.GetInternalState();
		if(!token.first) 
			continue;

		// skip comments
		if(IsComment(token.second))
			continue;

		// "end" encountered, dictionary finished, return.
		if(token.second.compare("end") == 0) 
			break;

		if(token.second.compare("/UniqueID") == 0)
		{
			mPrivateDictionary.UniqueID = Int(mPFBDecoder.GetNextToken().second);
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
			mPrivateDictionary.BlueScale = Double(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/BlueShift") == 0)
		{
			mPrivateDictionary.BlueShift = Int(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/BlueFuzz") == 0)
		{
			mPrivateDictionary.BlueFuzz = Int(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/StdHW") == 0)
		{
			mPFBDecoder.GetNextToken(); // skip [
			mPrivateDictionary.StdHW = Double(mPFBDecoder.GetNextToken().second);
			mPFBDecoder.GetNextToken(); // skip ]
			continue;
		}
		if(token.second.compare("/StdVW") == 0)
		{
			mPFBDecoder.GetNextToken(); // skip [
			mPrivateDictionary.StdVW = Double(mPFBDecoder.GetNextToken().second);
			mPFBDecoder.GetNextToken(); // skip ]
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
			mPrivateDictionary.ForceBold = PSBool(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/LanguageGroup") == 0)
		{
			mPrivateDictionary.LanguageGroup = Int(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/lenIV") == 0)
		{
			mPrivateDictionary.lenIV = Int(mPFBDecoder.GetNextToken().second);
			continue;
		}
		if(token.second.compare("/RndStemUp") == 0)
		{
			mPrivateDictionary.RndStemUp = PSBool(mPFBDecoder.GetNextToken().second);
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
			continue;

		}
	}
	return status;	
}

EStatusCode Type1Input::ParseIntVector(vector<int>& inVector)
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

		inVector.push_back(Int(token.second));
	}
	return token.first ? eSuccess:eFailure;
}

EStatusCode Type1Input::ParseDoubleVector(vector<double>& inVector)
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

		inVector.push_back(Double(token.second));
	}
	return token.first ? eSuccess:eFailure;
}

EStatusCode Type1Input::ParseSubrs()
{
	int subrIndex;

	// get the subrs count
	BoolAndString token = mPFBDecoder.GetNextToken();
	if(!token.first)
		return eFailure;

	mSubrsCount = (Byte)Int(token.second);
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
		return eFailure;

	for(Byte i=0;i<mSubrsCount && token.first;++i)
	{
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
			break;
				
		subrIndex = Int(token.second);
		token = mPFBDecoder.GetNextToken();
		if(!token.first)
			break;

		mSubrs[subrIndex].CodeLength = Int(token.second);
		mSubrs[subrIndex].Code = new Byte[mSubrs[subrIndex].CodeLength];

		// skip the RD token (will also skip space)
		mPFBDecoder.GetNextToken();

		mPFBDecoder.Read(mSubrs[subrIndex].Code,mSubrs[subrIndex].CodeLength);

		// skip NP token
		mPFBDecoder.GetNextToken();

		// skip dup or end array definition
		mPFBDecoder.GetNextToken();
	}
	if(!token.first)
		return eFailure;

	return mPFBDecoder.GetInternalState();
}

EStatusCode Type1Input::ParseCharstrings()
{
	BoolAndString token;
	string characterName;
	Type1CharString charString;

	// skip till "begin"
	while(mPFBDecoder.NotEnded())
	{
		token = mPFBDecoder.GetNextToken();
		if(!token.first || token.second.compare("begin") == 0)
			break;
	}
	if(!token.first)
		return eFailure;

	// Charstrings look like this:
	// charactername nbytes RD ~n~binary~bytes~ ND
	while(token.first && mPFBDecoder.GetInternalState() == eSuccess)
	{
		token = mPFBDecoder.GetNextToken();

		if("end" == token.second)
			break;

		characterName = FromPSName(token.second);

		charString.CodeLength = Int(mPFBDecoder.GetNextToken().second);

		charString.Code = new Byte[charString.CodeLength];

		// skip the RD token (will also skip space)
		mPFBDecoder.GetNextToken();


		mPFBDecoder.Read(charString.Code,charString.CodeLength);

		mCharStrings.insert(StringToType1CharStringMap::value_type(characterName,charString));

		// skip ND token
		mPFBDecoder.GetNextToken();
	}

	return mPFBDecoder.GetInternalState();
}

Type1CharString* Type1Input::GetGlyphCharString(Byte inCharStringIndex)
{
	if(256 <= inCharStringIndex)
	{
		TRACE_LOG1("Type1Input::GetGlyphCharString, inCharStringIndex = %d is invalid",inCharStringIndex);
		return NULL;
	}
	string characterName;

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

Type1CharString* Type1Input::GetGlyphCharString(const string& inCharStringName)
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

EStatusCode Type1Input::CalculateDependenciesForCharIndex(const string& inCharStringName,
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


Type1CharString* Type1Input::GetSubr(long inSubrIndex)
{
	if(mCurrentDependencies)
		mCurrentDependencies->mSubrs.insert((unsigned short)inSubrIndex);

	if(inSubrIndex >= mSubrsCount)
	{
		TRACE_LOG2("CharStringType1Tracer::GetLocalSubr exception, asked for %ld and there are only %ld count subrs",inSubrIndex,mSubrsCount);
		return NULL;
	}
	else
		return mSubrs+inSubrIndex;
}

EStatusCode Type1Input::Type1Seac(const LongList& inOperandList)
{
	LongList::const_reverse_iterator it = inOperandList.rbegin();

	mCurrentDependencies->mCharCodes.insert((Byte)*it);
	++it;
	mCurrentDependencies->mCharCodes.insert((Byte)*it);
	return eSuccess;
}

bool Type1Input::IsOtherSubrSupported(long inOtherSubrsIndex)
{
	mCurrentDependencies->mOtherSubrs.insert((unsigned short)inOtherSubrsIndex);
	return false;
}

unsigned long Type1Input::GetLenIV()
{
	return mPrivateDictionary.lenIV;
}

bool Type1Input::IsValidGlyphIndex(Byte inCharStringIndex)
{
	string characterName;

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

string Type1Input::GetGlyphCharStringName(Byte inCharStringIndex)
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