/*
   Source File : CFFFileInput.cpp


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
#include "CFFFileInput.h"
#include "Trace.h"
#include "CharStringType2Interpreter.h"
#include "StandardEncoding.h"

#include <algorithm>
#include <climits>
#include <stdint.h>


using namespace PDFHummus;

// Recursion depth cap for the Type 2 seac (4-arg endchar) dependency walk
// in CollectComponentGlyphs. The Adobe Type 2 spec disallows nested seac
// (bchar/achar must not be seac characters), so 16 is well beyond any
// conforming font but matches the TrueType composite cap used elsewhere
// in the codebase. Lenient on malformed-but-benign fonts; still bounds
// the call stack against attacker input.
static const unsigned int scMaxCompositeDepth = 16;

#define N_STD_STRINGS 391
static const char* scStandardStrings[N_STD_STRINGS] = {
		".notdef","space","exclam","quotedbl","numbersign","dollar","percent","ampersand","quoteright","parenleft",
		"parenright","asterisk","plus","comma","hyphen","period","slash","zero","one","two",
		"three","four","five","six","seven","eight","nine","colon","semicolon","less",
		"equal","greater","question","at","A","B","C","D","E","F",
		"G","H","I","J","K","L","M","N","O","P",
		"Q","R","S","T","U","V","W","X","Y","Z",
		"bracketleft","backslash","bracketright","asciicircum","underscore","quoteleft","a","b","c","d",
		"e","f","g","h","i","j","k","l","m","n",
		"o","p","q","r","s","t","u","v","w","x",
		"y","z","braceleft","bar","braceright","asciitilde","exclamdown","cent","sterling","fraction",
		"yen","florin","section","currency","quotesingle","quotedblleft","guillemotleft","guilsinglleft","guilsinglright","fi",
		"fl","endash","dagger","daggerdbl","periodcentered","paragraph","bullet","quotesinglbase","quotedblbase","quotedblright",
		"guillemotright","ellipsis","perthousand","questiondown","grave","acute","circumflex","tilde","macron","breve",
		"dotaccent","dieresis","ring","cedilla","hungarumlaut","ogonek","caron","emdash","AE","ordfeminine",
		"Lslash","Oslash","OE","ordmasculine","ae","dotlessi","lslash","oslash","oe","germandbls",
		"onesuperior","logicalnot","mu","trademark","Eth","onehalf","plusminus","Thorn","onequarter","divide",
		"brokenbar","degree","thorn","threequarters","twosuperior","registered","minus","eth","multiply","threesuperior",
		"copyright","Aacute","Acircumflex","Adieresis","Agrave","Aring","Atilde","Ccedilla","Eacute","Ecircumflex",
		"Edieresis","Egrave","Iacute","Icircumflex","Idieresis","Igrave","Ntilde","Oacute","Ocircumflex","Odieresis",
		"Ograve","Otilde","Scaron","Uacute","Ucircumflex","Udieresis","Ugrave","Yacute","Ydieresis","Zcaron",
		"aacute","acircumflex","adieresis","agrave","aring","atilde","ccedilla","eacute","ecircumflex","edieresis",
		"egrave","iacute","icircumflex","idieresis","igrave","ntilde","oacute","ocircumflex","odieresis","ograve",
        "otilde","scaron","uacute","ucircumflex","udieresis","ugrave","yacute","ydieresis","zcaron","exclamsmall",
		"Hungarumlautsmall","dollaroldstyle","dollarsuperior","ampersandsmall","Acutesmall","parenleftsuperior","parenrightsuperior","twodotenleader","onedotenleader","zerooldstyle",
		"oneoldstyle","twooldstyle","threeoldstyle","fouroldstyle","fiveoldstyle","sixoldstyle","sevenoldstyle","eightoldstyle","nineoldstyle","commasuperior",
		"threequartersemdash","periodsuperior","questionsmall","asuperior","bsuperior","centsuperior","dsuperior","esuperior","isuperior","lsuperior",
		"msuperior","nsuperior","osuperior","rsuperior","ssuperior","tsuperior","ff","ffi","ffl","parenleftinferior",
		"parenrightinferior","Circumflexsmall","hyphensuperior","Gravesmall","Asmall","Bsmall","Csmall","Dsmall","Esmall","Fsmall",
		"Gsmall","Hsmall","Ismall","Jsmall","Ksmall","Lsmall","Msmall","Nsmall","Osmall","Psmall",
		"Qsmall","Rsmall","Ssmall","Tsmall","Usmall","Vsmall","Wsmall","Xsmall","Ysmall","Zsmall",
		"colonmonetary","onefitted","rupiah","Tildesmall","exclamdownsmall","centoldstyle","Lslashsmall","Scaronsmall","Zcaronsmall","Dieresissmall",
		"Brevesmall","Caronsmall","Dotaccentsmall","Macronsmall","figuredash","hypheninferior","Ogoneksmall","Ringsmall","Cedillasmall","questiondownsmall",
		"oneeighth","threeeighths","fiveeighths","seveneighths","onethird","twothirds","zerosuperior","foursuperior","fivesuperior","sixsuperior",
		"sevensuperior","eightsuperior","ninesuperior","zeroinferior","oneinferior","twoinferior","threeinferior","fourinferior","fiveinferior","sixinferior",
		"seveninferior","eightinferior","nineinferior","centinferior","dollarinferior","periodinferior","commainferior","Agravesmall","Aacutesmall","Acircumflexsmall",
		"Atildesmall","Adieresissmall","Aringsmall","AEsmall","Ccedillasmall","Egravesmall","Eacutesmall","Ecircumflexsmall","Edieresissmall","Igravesmall",
		"Iacutesmall","Icircumflexsmall","Idieresissmall","Ethsmall","Ntildesmall","Ogravesmall","Oacutesmall","Ocircumflexsmall","Otildesmall","Odieresissmall",
		"OEsmall","Oslashsmall","Ugravesmall","Uacutesmall","Ucircumflexsmall","Udieresissmall","Yacutesmall","Thornsmall","Ydieresissmall","001.000",
		"001.001","001.002","001.003","Black","Bold","Book","Light","Medium","Regular","Roman",
		"Semibold"
};

#define CHARSET_ISOADOBE_SIZE 228
static const unsigned short scCharsetIsoadobeSids[CHARSET_ISOADOBE_SIZE] = 
{
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,
	78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,
	111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,
	138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,
	165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
	192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,
	219,220,221,222,223,224,225,226,227,228
};

#define CHARSET_EXPERT_SIZE 165
static const unsigned short scCharsetExpertSids[CHARSET_EXPERT_SIZE] = 
{
	1,229,230,231,232,233,234,235,236,237,238,13,14,15,99,239,240,241,242,243,244,245,246,247,248,27,28,
	249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,109,110,267,268,269,270,271,272,
	273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,
	299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,158,155,163,319,320,321,
	322,323,324,325,326,150,164,169,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344, 
	345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,
	370,371,372,373,374,375,376,377,378
};

#define CHARSET_EXPERT_SUBSET_SIZE 86
static const unsigned short scCharsetExpertSubsetSids[CHARSET_EXPERT_SUBSET_SIZE] = 
{
	1,231,232,235,236,237,238,13,14,15,99,239,240,241,242,243,244,245,246,247,248,27,28,
	249,250,251,253,254,255,256,257,258,259,260,261,262,263,264,265,266,109,110,267,268,
	269,270,272,300,301,302,305,314,315,158,155,163,320,321,322,323,324,325,326,150,164,
	169,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346
};

static const unsigned short* scDefaultCharsets[3] = 
{
	scCharsetIsoadobeSids,
	scCharsetExpertSids,
	scCharsetExpertSubsetSids
};

static const unsigned short scDefaultCharsetsSizes[3] = 
{
	CHARSET_ISOADOBE_SIZE,
	CHARSET_EXPERT_SIZE,
	CHARSET_EXPERT_SUBSET_SIZE
};

CFFFileInput::CFFFileInput(void)
{
	mFontsCount = 0;
	mStringsCount = 0;
	mTopDictIndex = NULL;
	mStrings = NULL;
	mGlobalSubrs.mCharStringsIndex = NULL;
	mCharStrings = NULL;
	mPrivateDicts = NULL;
	// Set only by PrepareForGlyphIntepretation; the dependency walkers and
	// GetCharacterFromStandardEncoding rely on NULL meaning "no active
	// interpretation flow".
	mCurrentCharsetInfo = NULL;
	mCurrentDependencies = NULL;
}

CFFFileInput::~CFFFileInput(void)
{
	FreeData();
}

void CFFFileInput::FreeData()
{
	mName.clear();
	mNameToIndex.clear();
	if(mTopDictIndex != NULL)
	{
		for(unsigned long i=0; i < mFontsCount;++i)
		{
			delete[] mTopDictIndex[i].mFDArray;
			delete[] mTopDictIndex[i].mFDSelect;
		}
		delete[] mTopDictIndex;
		mTopDictIndex = NULL;
	}
	if(mStrings != NULL)
	{
		for(unsigned long i =0 ; i <mStringsCount; ++ i)
			delete[] mStrings[i];
		delete[] mStrings;
		mStrings = NULL;
	}
	mStringToSID.clear();
	delete[] mGlobalSubrs.mCharStringsIndex;
	mGlobalSubrs.mCharStringsIndex = NULL;
	if(mCharStrings != NULL)
	{
		for(unsigned long i=0; i < mFontsCount; ++i)
			delete[] mCharStrings[i].mCharStringsIndex;
		delete mCharStrings;
		mCharStrings = NULL;
	}
	delete[] mPrivateDicts;
	mPrivateDicts = NULL;

	LongFilePositionTypeToCharStringsMap::iterator itLocalSubrs = mLocalSubrs.begin();
	for(; itLocalSubrs != mLocalSubrs.end(); ++itLocalSubrs)
	{
		delete[] itLocalSubrs->second->mCharStringsIndex;
		delete itLocalSubrs->second;
	}

	CharSetInfoVector::iterator itCharSets = mCharSets.begin();
	for(; itCharSets != mCharSets.end(); ++itCharSets)
	{
		delete[] (*itCharSets)->mSIDs;
		(*itCharSets)->mSIDToGlyphMap.clear();
		delete (*itCharSets);
	}
	mCharSets.clear();

	EncodingsInfoVector::iterator itEncodings = mEncodings.begin();
	for(; itEncodings != mEncodings.end(); ++itEncodings)
	{
		delete[] (*itEncodings)->mEncoding;
		delete (*itEncodings);
	}
	mEncodings.clear();
}

void CFFFileInput::Reset()
{
	FreeData();
}

EStatusCode CFFFileInput::ReadCFFFile(IByteReaderWithPosition* inCFFFile)
{
	EStatusCode status;

	do
	{
		FreeData();

		mPrimitivesReader.SetStream(inCFFFile);
		mCFFOffset = inCFFFile->GetCurrentPosition();

		status = ReadHeader();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile, Failed to read header");
				break;
		}

		// skip to name
		if(mHeader.hdrSize > 4)
			mPrimitivesReader.Skip(mHeader.hdrSize - 4);

		status = ReadNameIndex();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile Failed to read name");
				break;
		}

		status = ReadTopDictIndex();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
			break;
		}

		status = ReadStringIndex();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
			break;
		}

		status = ReadGlobalSubrs();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read global subrs");
			break;
		}

		status = ReadCharStrings();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
			break;
		}

		status = ReadPrivateDicts();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
			break;
		}

		status = ReadLocalSubrs();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read local subrs");
			break;
		}

		status = ReadCharsets();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read char set");
			break;
		}

		status = ReadEncodings();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read encodings");
			break;
		}
		
		status = ReadCIDInformation();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read CID Information");
			break;
		}

	}while(false);
	return status;
}

EStatusCode CFFFileInput::ReadHeader()
{
	mPrimitivesReader.ReadCard8(mHeader.major);
	mPrimitivesReader.ReadCard8(mHeader.minor);
	mPrimitivesReader.ReadCard8(mHeader.hdrSize);
	mPrimitivesReader.ReadOffSize(mHeader.offSize);

	return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadIndexHeader(unsigned long** outOffsets,unsigned short& outItemsCount)
{
	Byte offSizeForIndex;
	EStatusCode status = PDFHummus::eFailure;

	*outOffsets = NULL;

	do
	{
		status = mPrimitivesReader.ReadCard16(outItemsCount);
		if(status != PDFHummus::eSuccess)
			break;

		if(0 == outItemsCount)
			break;

		status = mPrimitivesReader.ReadOffSize(offSizeForIndex);
		if(status != PDFHummus::eSuccess)
			break;

		// CFF spec requires offSize ∈ {1, 2, 3, 4}.
		if(offSizeForIndex < 1 || offSizeForIndex > 4)
		{
			TRACE_LOG1("CFFFileInput::ReadIndexHeader, invalid INDEX offSize %d (must be 1..4)", offSizeForIndex);
			status = PDFHummus::eFailure;
			break;
		}

		mPrimitivesReader.SetOffSize(offSizeForIndex);
		*outOffsets = new unsigned long[outItemsCount + 1];

		for(unsigned long i = 0; i <= outItemsCount && status == eSuccess; ++ i)
			status = mPrimitivesReader.ReadOffset((*outOffsets)[i]);
		if(status != eSuccess)
			break;

		// offsets[0] must be >= 1: callers do Skip(offsets[0] - 1) to advance past
		// pre-data padding, and offsets[0] == 0 would underflow that subtraction.
		if((*outOffsets)[0] < 1)
		{
			TRACE_LOG1("CFFFileInput::ReadIndexHeader, INDEX offsets[0] = %lu (must be >= 1; would underflow Skip)", (*outOffsets)[0]);
			status = PDFHummus::eFailure;
			break;
		}

		// Offsets must be monotonically non-decreasing.
		for(unsigned long i = 0; i < outItemsCount; ++i)
		{
			if((*outOffsets)[i+1] < (*outOffsets)[i])
			{
				TRACE_LOG3("CFFFileInput::ReadIndexHeader, non-monotonic INDEX offset at i=%lu: %lu -> %lu",
					i, (*outOffsets)[i], (*outOffsets)[i+1]);
				status = PDFHummus::eFailure;
				break;
			}
		}
		if(status != PDFHummus::eSuccess)
			break;

		status = mPrimitivesReader.GetInternalState();
	} while(false);

	if(status != PDFHummus::eSuccess)
	{
		delete[] *outOffsets;
		*outOffsets = NULL;
	}

	return status;
}

EStatusCode CFFFileInput::ReadNameIndex()
{
	mNameIndexPosition = mPrimitivesReader.GetCurrentPosition();
	
	unsigned long* offsets;
	EStatusCode status = ReadIndexHeader(&offsets,mFontsCount);
	Byte* buffer;
	
	do
	{
		if(status != PDFHummus::eSuccess)
			break;

		if(offsets[0] != 1)
			mPrimitivesReader.Skip(offsets[0] - 1);

		for(unsigned long i = 0; i < mFontsCount; ++i)
		{
			buffer = new Byte[offsets[i+1] - offsets[i]];
			mPrimitivesReader.Read(buffer,offsets[i+1] - offsets[i]);
			std::string aName((char*)buffer,offsets[i+1] - offsets[i]);
			mName.push_back(aName);
			if(buffer[0] != 0) // put in map only valid names
				mNameToIndex.insert(StringToUShort::value_type(aName,i));
			delete[] buffer;
		}

	}while(false);

	delete[] offsets;
	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

BoolAndUShort CFFFileInput::GetIndexForFontName(const std::string& inFontName)
{
	StringToUShort::iterator it = mNameToIndex.find(inFontName);

	if(it == mNameToIndex.end())
		return BoolAndUShort(false,0);
	else
		return BoolAndUShort(true,it->second);
}

EStatusCode CFFFileInput::ReadTopDictIndex()
{
	mTopDictIndexPosition = mPrimitivesReader.GetCurrentPosition();
	
	unsigned long* offsets;
	unsigned short dictionariesCount;
	EStatusCode status = ReadIndexHeader(&offsets,dictionariesCount);
	
	do
	{
		if(status != PDFHummus::eSuccess)
			break;

		if(offsets[0] != 1)
			mPrimitivesReader.Skip(offsets[0] - 1);

		mTopDictIndex = new TopDictInfo[dictionariesCount];

		for(unsigned long i = 0; i < dictionariesCount && (PDFHummus::eSuccess == status); ++i)
			status = ReadDict(offsets[i+1] - offsets[i],mTopDictIndex[i].mTopDict);

	}while(false);

	delete[] offsets;
	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadDict(unsigned long inReadAmount,UShortToDictOperandListMap& outDict)
{
	LongFilePositionType dictStartPosition = mPrimitivesReader.GetCurrentPosition();
	DictOperandList operands;
	EStatusCode status = PDFHummus::eSuccess;
	unsigned short anOperator;
	DictOperand anOperand;
	Byte aBuffer;

	while(PDFHummus::eSuccess == status && 
			(mPrimitivesReader.GetCurrentPosition() - dictStartPosition < (long long)inReadAmount))
	{
		status = mPrimitivesReader.ReadByte(aBuffer);
		if(status != PDFHummus::eSuccess)
			break;
		if(mPrimitivesReader.IsDictOperator(aBuffer))
		{ // operator
			status = mPrimitivesReader.ReadDictOperator(aBuffer,anOperator);
			if(status != PDFHummus::eSuccess)
				break;
			outDict.insert(
				UShortToDictOperandListMap::value_type(anOperator,operands));
			operands.clear();
		}
		else // operand
		{
			status = mPrimitivesReader.ReadDictOperand(aBuffer,anOperand);
			if(status != PDFHummus::eSuccess)
				break;
			operands.push_back(anOperand);
		}
	}

	return status;
}

EStatusCode CFFFileInput::ReadStringIndex()
{
	mStringIndexPosition = mPrimitivesReader.GetCurrentPosition();
	
	unsigned long* offsets;
	EStatusCode status = ReadIndexHeader(&offsets,mStringsCount);

	do
	{
		if(status != PDFHummus::eSuccess)
			break;

		unsigned long i;

		if(mStringsCount > 0)
		{
			if(offsets[0] != 1)
				mPrimitivesReader.Skip(offsets[0] - 1);

			mStrings = new char*[mStringsCount];

			for(i = 0; i < mStringsCount && (PDFHummus::eSuccess == status); ++i)
			{
				mStrings[i] = new char[offsets[i+1] - offsets[i]+1];
				status = mPrimitivesReader.Read((Byte*)mStrings[i],offsets[i+1] - offsets[i]);
				if(status != PDFHummus::eSuccess)
					break;
				mStrings[i][offsets[i+1] - offsets[i]] = 0;
			}

			// failure case, null all the rest of the strings for later delete to not perofrm errors
			if(status != PDFHummus::eSuccess)
			{	
				for(;i<mStringsCount;++i)
					mStrings[i] = NULL;
			}
		}
		else
		{
			mStrings = NULL;
		}

		// now create the string to SID map
		for(i=0; i < N_STD_STRINGS;++i)
			mStringToSID.insert(CharPToUShortMap::value_type(scStandardStrings[i],i));
		for(;i < N_STD_STRINGS + mStringsCount; ++ i)
			mStringToSID.insert(CharPToUShortMap::value_type(mStrings[i-N_STD_STRINGS],i));

	}while(false);

	delete[] offsets;
	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

std::string CFFFileInput::GetStringForSID(unsigned short inSID)
{
	if(inSID < N_STD_STRINGS)
		return scStandardStrings[inSID];
	else if(inSID < N_STD_STRINGS + mStringsCount)
		return mStrings[inSID - N_STD_STRINGS];
	else
		return "";
}

EStatusCode CFFFileInput::ReadGlobalSubrs()
{
	mGlobalSubrsPosition = mPrimitivesReader.GetCurrentPosition();
	mGlobalSubrs.mCharStringsType = 2; // always 2
	return ReadSubrsFromIndex(mGlobalSubrs.mCharStringsCount,&(mGlobalSubrs.mCharStringsIndex));
}

EStatusCode CFFFileInput::ReadSubrsFromIndex(unsigned short& outSubrsCount,
											CharStringsIndex* outSubrsIndex)
{
	unsigned long* offsets;
	EStatusCode status = ReadIndexHeader(&offsets,outSubrsCount);
		
	do
	{
		if(status != PDFHummus::eSuccess)
			break;

		if(0 == outSubrsCount)
		{
			*outSubrsIndex = NULL;
			break;
		}

		// just taking a snapshot of positions here
		*outSubrsIndex = new CharString[outSubrsCount];
		
		LongFilePositionType dataStartPosition = mPrimitivesReader.GetCurrentPosition();

		for(unsigned long i = 0; i < outSubrsCount; ++i)
		{
			(*outSubrsIndex)[i].mStartPosition = dataStartPosition + offsets[i] - 1;
			(*outSubrsIndex)[i].mEndPosition = dataStartPosition + offsets[i+1] - 1;
			(*outSubrsIndex)[i].mIndex = i;
		}

		// for good faith put the pointer at the end now (if someone wants to take a snapshot)
		mPrimitivesReader.Skip(offsets[outSubrsCount] - 1);
	}while(false);

	delete[] offsets;
	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadCharStrings()
{
	// scan all charstrings of all included fonts
	mCharStrings = new CharStrings[mFontsCount];
	EStatusCode status = PDFHummus::eSuccess;

	for(unsigned long i=0; i < mFontsCount && (PDFHummus::eSuccess == status); ++i)
	{
		LongFilePositionType  charStringsPosition = GetCharStringsPosition(i);	
		mCharStrings[i].mCharStringsType = (Byte)GetCharStringType(i);
		if(0 == charStringsPosition)
		{
			mCharStrings[i].mCharStringsCount = 0;
			mCharStrings[i].mCharStringsIndex = NULL;
		}
		else
		{
			mPrimitivesReader.SetOffset(charStringsPosition);
			status = ReadSubrsFromIndex(mCharStrings[i].mCharStringsCount,
										&(mCharStrings[i].mCharStringsIndex));
		}
	}

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

static const unsigned short scCharStringsKey = 17;
LongFilePositionType CFFFileInput::GetCharStringsPosition(unsigned short inFontIndex)
{
	return GetSingleIntegerValue(inFontIndex,scCharStringsKey,0);
}	

long CFFFileInput::GetSingleIntegerValue(unsigned short inFontIndex,unsigned short inKey,long inDefault)
{
	return GetSingleIntegerValueFromDict(mTopDictIndex[inFontIndex].mTopDict,inKey,inDefault);
}

long CFFFileInput::GetSingleIntegerValueFromDict(const UShortToDictOperandListMap& inDict,
												 unsigned short inKey,
												 long inDefault)
{
	UShortToDictOperandListMap::const_iterator it = inDict.find(inKey);

	// Every caller treats the returned value as a non-negative quantity
	// (file offset, intra-dict offset, or enum-like type id). Empty list,
	// non-integer, or negative integer all fall back to the supplied
	// default so garbage from front() / the union / a malicious negative
	// doesn't flow into SetOffset / INDEX read amounts downstream.
	if(it == inDict.end() || it->second.empty() || !it->second.front().IsInteger
		|| it->second.front().IntegerValue < 0)
		return inDefault;

	return it->second.front().IntegerValue;
}

static const unsigned short scCharstringType = 0x0C06;
long CFFFileInput::GetCharStringType(unsigned short inFontIndex)
{
	return GetSingleIntegerValue(inFontIndex,scCharstringType,2);
}

EStatusCode CFFFileInput::ReadPrivateDicts()
{
	mPrivateDicts = new PrivateDictInfo[mFontsCount];
	EStatusCode status = PDFHummus::eSuccess;

	for(unsigned long i=0; i < mFontsCount && (PDFHummus::eSuccess == status); ++i)
		status = ReadPrivateDict(mTopDictIndex[i].mTopDict,mPrivateDicts + i);

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();	
}

static const unsigned short scPrivate = 18;
EStatusCode CFFFileInput::ReadPrivateDict(const UShortToDictOperandListMap& inReferencingDict,PrivateDictInfo* outPrivateDict)
{
	EStatusCode status = PDFHummus::eSuccess;
	UShortToDictOperandListMap::const_iterator it = inReferencingDict.find(scPrivate);

	outPrivateDict->mLocalSubrs = NULL;
	if(it == inReferencingDict.end())
	{
		outPrivateDict->mPrivateDictStart = 0;
		outPrivateDict->mPrivateDictEnd = 0;
	}
	else
	{
		// /Private operand list is (size, offset) — two integers. Reject
		// fewer operands or non-integer reals before they flow into
		// SetOffset and ReadDict's read amount.
		if(it->second.size() < 2)
		{
			TRACE_LOG1("CFFFileInput::ReadPrivateDict, /Private has %lu operands (need at least 2)",
				(unsigned long)it->second.size());
			return PDFHummus::eFailure;
		}
		const DictOperand& sizeOperand = it->second.front();
		const DictOperand& offsetOperand = it->second.back();
		if(!sizeOperand.IsInteger || !offsetOperand.IsInteger)
		{
			TRACE_LOG("CFFFileInput::ReadPrivateDict, /Private size and offset must be integers");
			return PDFHummus::eFailure;
		}
		// Negative size would convert to a huge value when passed to
		// ReadDict's unsigned read amount; negative offset would seek to
		// junk. Both are malformed for what the spec defines as a byte
		// position and a byte count.
		if(sizeOperand.IntegerValue < 0 || offsetOperand.IntegerValue < 0)
		{
			TRACE_LOG2("CFFFileInput::ReadPrivateDict, /Private size=%ld and offset=%ld must be non-negative",
				sizeOperand.IntegerValue, offsetOperand.IntegerValue);
			return PDFHummus::eFailure;
		}

		outPrivateDict->mPrivateDictStart = (LongFilePositionType)offsetOperand.IntegerValue;
		// Add in LongFilePositionType (signed 64-bit) so the sum can't
		// overflow `long` on platforms where it's 32-bit.
		outPrivateDict->mPrivateDictEnd = (LongFilePositionType)offsetOperand.IntegerValue +
		                                  (LongFilePositionType)sizeOperand.IntegerValue;

		mPrimitivesReader.SetOffset(offsetOperand.IntegerValue);
		status = ReadDict(sizeOperand.IntegerValue,outPrivateDict->mPrivateDict);
	}
	return status;
}

EStatusCode CFFFileInput::ReadLocalSubrs()
{
	// scan all subrs of all included fonts
	EStatusCode status = PDFHummus::eSuccess;

	for(unsigned long i=0; i < mFontsCount && (PDFHummus::eSuccess == status); ++i)
		status = ReadLocalSubrsForPrivateDict(mPrivateDicts+i,(Byte)GetCharStringType(i));

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

static const unsigned short scSubrs = 19;
EStatusCode CFFFileInput::ReadLocalSubrsForPrivateDict(PrivateDictInfo* inPrivateDict,Byte inCharStringType)
{
	EStatusCode status = PDFHummus::eSuccess;
	LongFilePositionType  subrsPosition = GetSingleIntegerValueFromDict(inPrivateDict->mPrivateDict,scSubrs,0);	

	if(0 == subrsPosition)
	{
		inPrivateDict->mLocalSubrs = NULL;
	}
	else
	{
		LongFilePositionTypeToCharStringsMap::iterator	it = mLocalSubrs.find(inPrivateDict->mPrivateDictStart + subrsPosition);
		if(it == mLocalSubrs.end())
		{
			CharStrings* charStrings = new CharStrings();
			charStrings->mCharStringsType = inCharStringType;
			mPrimitivesReader.SetOffset(inPrivateDict->mPrivateDictStart + subrsPosition);
			status = ReadSubrsFromIndex(charStrings->mCharStringsCount,
										&(charStrings->mCharStringsIndex));
			if(status != PDFHummus::eSuccess)
				TRACE_LOG("CFFFileInput::ReadLocalSubrs, failed to read local subrs");
			else
				it = mLocalSubrs.insert(LongFilePositionTypeToCharStringsMap::value_type(inPrivateDict->mPrivateDictStart + subrsPosition,charStrings)).first;
		}
		inPrivateDict->mLocalSubrs = it->second;
	}
	return status;
}

typedef std::map<LongFilePositionType,CharSetInfo*> LongFilePositionTypeToCharSetInfoMap;

static const unsigned short scROS = 0xC1E;
EStatusCode CFFFileInput::ReadCharsets()
{
	// read all charsets
	EStatusCode status = PDFHummus::eSuccess;
	LongFilePositionTypeToCharSetInfoMap offsetToIndex;
	LongFilePositionTypeToCharSetInfoMap::iterator it;

	for(unsigned long i=0; i < mFontsCount && (PDFHummus::eSuccess == status); ++i)
	{
		LongFilePositionType charsetPosition = GetCharsetPosition(i);
		it = offsetToIndex.find(charsetPosition);
		if(it == offsetToIndex.end())
		{
			CharSetInfo* charSet = new CharSetInfo();
			bool isCID = mTopDictIndex[i].mTopDict.find(scROS) != mTopDictIndex[i].mTopDict.end();
			if(charsetPosition <= 2)
			{
				charSet->mType = (ECharSetType)charsetPosition;
				charSet->mSIDs = NULL;
				if(!isCID) // collect SID->Glyph map only if not CID, in which case SIDs are CIDs...and what i'm using the map for is irrelevant
					SetupSIDToGlyphMapWithStandard(scDefaultCharsets[charsetPosition],
												   scDefaultCharsetsSizes[charsetPosition],
												   charSet->mSIDToGlyphMap,
												   mCharStrings[i]);
			}
			else
			{
				Byte charsetFormat;
				charSet->mType = eCharSetCustom;
				mPrimitivesReader.SetOffset(charsetPosition);
				mPrimitivesReader.ReadCard8(charsetFormat);

				if(0 == charsetFormat)
					status = ReadFormat0Charset(isCID, charSet->mSIDToGlyphMap,
												&charSet->mSIDs,
												mCharStrings[i]);
				else if(1 == charsetFormat)
					status = ReadFormat1Charset(isCID, charSet->mSIDToGlyphMap,
												&charSet->mSIDs,
												mCharStrings[i]);
				else // 2 charset format
					status = ReadFormat2Charset(isCID, charSet->mSIDToGlyphMap,
												&charSet->mSIDs,
												mCharStrings[i]);
			}
			mCharSets.push_back(charSet);
			it = offsetToIndex.insert(LongFilePositionTypeToCharSetInfoMap::value_type(charsetPosition,charSet)).first;
		}
		mTopDictIndex[i].mCharSet = it->second;
	}

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}


typedef std::map<LongFilePositionType,EncodingsInfo*> LongFilePositionTypeToEncodingsInfoMap;


EStatusCode CFFFileInput::ReadEncodings()
{
	// read all encodings positions
	EStatusCode status = PDFHummus::eSuccess;
	LongFilePositionTypeToEncodingsInfoMap offsetToEncoding;
	LongFilePositionTypeToEncodingsInfoMap::iterator it;

	for(unsigned long i=0; i < mFontsCount; ++i)
	{
		LongFilePositionType encodingPosition = GetEncodingPosition(i);
		it = offsetToEncoding.find(encodingPosition);
		if(it == offsetToEncoding.end())
		{
			EncodingsInfo* encoding = new EncodingsInfo();
			status = ReadEncoding(encoding,encodingPosition);
			mEncodings.push_back(encoding);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG1("CFFFileInput::ReadEncodings, failed to read encoding for font index %lu", i);
				break;
			}
			it = offsetToEncoding.insert(LongFilePositionTypeToEncodingsInfoMap::value_type(encodingPosition,encoding)).first;
		}
		mTopDictIndex[i].mEncoding = it->second;
	}

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();

}

EStatusCode CFFFileInput::ReadEncoding(EncodingsInfo* inEncoding,LongFilePositionType inEncodingPosition)
{
	if(inEncodingPosition <= 1)
	{
		inEncoding->mEncodingStart = inEncoding->mEncodingEnd = inEncodingPosition;
		inEncoding->mType = (EEncodingType)inEncodingPosition;
		return PDFHummus::eSuccess;
	}

	inEncoding->mType = eEncodingCustom;
	Byte encodingFormat = 0;
	inEncoding->mEncodingStart = inEncodingPosition;
	mPrimitivesReader.SetOffset(inEncodingPosition);
	mPrimitivesReader.ReadCard8(encodingFormat);

	if(0 == (encodingFormat & 0x1))
	{
		Byte rawCount = 0;
		mPrimitivesReader.ReadCard8(rawCount);
		inEncoding->mEncodingsCount = rawCount;
		if(inEncoding->mEncodingsCount > 0)
		{
			inEncoding->mEncoding = new Byte[inEncoding->mEncodingsCount];
			for(unsigned short i=0; i < inEncoding->mEncodingsCount; ++i)
				mPrimitivesReader.ReadCard8(inEncoding->mEncoding[i]);
		}
	}
	else // format = 1
	{
		Byte rangesCount = 0;
		mPrimitivesReader.ReadCard8(rangesCount);
		if(rangesCount > 0)
		{
			Byte firstCode;
			Byte left;

			// First pass: sum range lengths into a 16-bit accumulator so the
			// total can't wrap before we cap it. Encoding covers codes 0..255,
			// so the spec ceiling on the sum is 256.
			unsigned short totalCount = 0;
			for(Byte i=0; i < rangesCount; ++i)
			{
				mPrimitivesReader.ReadCard8(firstCode);
				mPrimitivesReader.ReadCard8(left);
				totalCount += left;
			}
			if(totalCount > 256)
			{
				TRACE_LOG1("CFFFileInput::ReadEncoding, format-1 range sum %u exceeds 256-code spec ceiling", totalCount);
				return PDFHummus::eFailure;
			}
			inEncoding->mEncodingsCount = totalCount;
			inEncoding->mEncoding = new Byte[inEncoding->mEncodingsCount];
			mPrimitivesReader.SetOffset(inEncodingPosition+2); // reset encoding to beginning of range reading

			// Second pass: encodingIndex must be 16-bit too — Byte += Byte wraps at 256
			// and the writes would then alias the start of the (correctly-sized) buffer.
			unsigned short encodingIndex = 0;
			for(Byte i=0; i < rangesCount; ++i)
			{
				mPrimitivesReader.ReadCard8(firstCode);
				mPrimitivesReader.ReadCard8(left);
				for(Byte j=0;j < left;++j)
					inEncoding->mEncoding[encodingIndex+j] = firstCode+j;
				encodingIndex+=left;
			}
		}
	}
	// Record the post-base-encoding position before the supplements block.
	// The supplements block reads from the current position, so anchoring
	// mEncodingEnd here keeps the struct field meaningful for callers and
	// also documents the position the previous code was trying to seek
	// back to (the prior SetOffset(mEncodingEnd) read mEncodingEnd before
	// it had ever been set on the custom-encoding path).
	inEncoding->mEncodingEnd = mPrimitivesReader.GetCurrentPosition();

	if((encodingFormat & 0x80) !=  0) // supplements exist, need to add to encoding end
	{
		Byte supplementsCount = 0;
		mPrimitivesReader.ReadCard8(supplementsCount);
		if(supplementsCount > 0)
		{
			Byte encoding;
			unsigned short SID;
			for(Byte i=0; i < supplementsCount; ++i)
			{
				mPrimitivesReader.ReadCard8(encoding);
				mPrimitivesReader.ReadCard16(SID);

				UShortToByteList::iterator it = inEncoding->mSupplements.find(SID);
				if(it == inEncoding->mSupplements.end())
					it = inEncoding->mSupplements.insert(UShortToByteList::value_type(SID,ByteList())).first;
				it->second.push_back(encoding);
			}
		}
		inEncoding->mEncodingEnd = mPrimitivesReader.GetCurrentPosition();
	}

	return mPrimitivesReader.GetInternalState();
}

void CFFFileInput::SetupSIDToGlyphMapWithStandard(	const unsigned short* inStandardCharSet,
													unsigned short inStandardCharSetLength,
													UShortToCharStringMap& ioCharMap,
													const CharStrings& inCharStrings)
{
	ioCharMap.insert(UShortToCharStringMap::value_type(0,inCharStrings.mCharStringsIndex));
	unsigned short i;
	for(i = 1; i < inCharStrings.mCharStringsCount && i - 1 < inStandardCharSetLength;++i)
	{
		ioCharMap.insert(UShortToCharStringMap::value_type(
				inStandardCharSet[i - 1],inCharStrings.mCharStringsIndex + i));
	}
}

EStatusCode CFFFileInput::ReadFormat0Charset(bool inIsCID,
											 UShortToCharStringMap& ioGlyphMap,
											 unsigned short** inSIDArray,
											 const CharStrings& inCharStrings)
{
	// for CIDs don't bother filling up the SID->glyph map. it ain't SIDs
	if(!inIsCID)
		ioGlyphMap.insert(UShortToCharStringMap::value_type(0,inCharStrings.mCharStringsIndex));
	*inSIDArray = new unsigned short[inCharStrings.mCharStringsCount];
	// element 0 is in range only when the CharStrings INDEX is non-empty;
	// new unsigned short[0] is a zero-length buffer.
	if(inCharStrings.mCharStringsCount > 0)
		(*inSIDArray)[0] = 0;

	if(inIsCID)
	{
		for(unsigned short i = 1; i < inCharStrings.mCharStringsCount;++i)
			mPrimitivesReader.ReadSID((*inSIDArray)[i]);
	}
	else
	{
		for(unsigned short i = 1; i < inCharStrings.mCharStringsCount;++i)
		{
			unsigned short sid;
			mPrimitivesReader.ReadSID(sid);
			(*inSIDArray)[i] = sid;

			ioGlyphMap.insert(UShortToCharStringMap::value_type(sid,inCharStrings.mCharStringsIndex + i));
		}
	}
	return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadFormat1Charset(bool inIsCID,
											 UShortToCharStringMap& ioGlyphMap,
											 unsigned short** inSIDArray,
											 const CharStrings& inCharStrings)
{
	if(!inIsCID)
		ioGlyphMap.insert(UShortToCharStringMap::value_type(0,inCharStrings.mCharStringsIndex));
	*inSIDArray = new unsigned short[inCharStrings.mCharStringsCount];
	// element 0 is in range only when the CharStrings INDEX is non-empty;
	// new unsigned short[0] is a zero-length buffer.
	if(inCharStrings.mCharStringsCount > 0)
		(*inSIDArray)[0] = 0;
	unsigned long glyphIndex = 1;
	unsigned short sid;
	Byte left;
	
	if(inIsCID)
	{
		while(glyphIndex < inCharStrings.mCharStringsCount)
		{
			mPrimitivesReader.ReadSID(sid);
			mPrimitivesReader.ReadCard8(left);
			for(unsigned long i=0; i <= left && glyphIndex < inCharStrings.mCharStringsCount;++i,++glyphIndex)
				(*inSIDArray)[glyphIndex] = sid+i;
		}
	}
	else
	{
		while(glyphIndex < inCharStrings.mCharStringsCount)
		{
			mPrimitivesReader.ReadSID(sid);
			mPrimitivesReader.ReadCard8(left);
			for(unsigned long i=0; i <= left && glyphIndex < inCharStrings.mCharStringsCount;++i,++glyphIndex)
			{
				ioGlyphMap.insert(
					UShortToCharStringMap::value_type(sid+i,inCharStrings.mCharStringsIndex + glyphIndex));
				(*inSIDArray)[glyphIndex] = sid+i;
			}
		}
	}
	return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadFormat2Charset(bool inIsCID,
											 UShortToCharStringMap& ioGlyphMap,
											 unsigned short** inSIDArray,
											 const CharStrings& inCharStrings)
{
	if(!inIsCID)
		ioGlyphMap.insert(UShortToCharStringMap::value_type(0,inCharStrings.mCharStringsIndex));
	*inSIDArray = new unsigned short[inCharStrings.mCharStringsCount];
	// element 0 is in range only when the CharStrings INDEX is non-empty;
	// new unsigned short[0] is a zero-length buffer.
	if(inCharStrings.mCharStringsCount > 0)
		(*inSIDArray)[0] = 0;
	unsigned short glyphIndex = 1;
	unsigned short sid;
	unsigned short left;
	
	if(inIsCID)
	{
		while(glyphIndex < inCharStrings.mCharStringsCount)
		{
			mPrimitivesReader.ReadSID(sid);
			mPrimitivesReader.ReadCard16(left);
			for(unsigned long i=0; i <= left && glyphIndex < inCharStrings.mCharStringsCount;++i,++glyphIndex)
				(*inSIDArray)[glyphIndex] = sid+i;
		}
	}
	else
	{
		while(glyphIndex < inCharStrings.mCharStringsCount)
		{
			mPrimitivesReader.ReadSID(sid);
			mPrimitivesReader.ReadCard16(left);
			for(unsigned long i=0; i <= left && glyphIndex < inCharStrings.mCharStringsCount;++i,++glyphIndex)
			{
				ioGlyphMap.insert(
					UShortToCharStringMap::value_type(sid+i,inCharStrings.mCharStringsIndex + glyphIndex));
				(*inSIDArray)[glyphIndex] = sid+i;
			}
		}
	}
	return mPrimitivesReader.GetInternalState();
}

static const unsigned short scCharset = 15;
LongFilePositionType CFFFileInput::GetCharsetPosition(unsigned short inFontIndex)
{
	return (LongFilePositionType)GetSingleIntegerValue(inFontIndex,scCharset,0);
}

static const unsigned short scEncoding = 16;
LongFilePositionType CFFFileInput::GetEncodingPosition(unsigned short inFontIndex)
{
	return (LongFilePositionType)GetSingleIntegerValue(inFontIndex,scEncoding,0);
}


EStatusCode CFFFileInput::CalculateDependenciesForCharIndex(unsigned short inFontIndex,
															unsigned short inCharStringIndex,
															CharString2Dependencies& ioDependenciesInfo)
{
	CharStringType2Interpreter interpreter;

	EStatusCode status = PrepareForGlyphIntepretation(inFontIndex,inCharStringIndex);
	if(status != PDFHummus::eFailure)
	{
		mCurrentDependencies = &ioDependenciesInfo;
		CharString* charString = GetGlyphCharString(inFontIndex,inCharStringIndex);
        if(!charString)
        {
            TRACE_LOG("GetGlyphCharString cannot find char string");
            return PDFHummus::eFailure;
        }
		return interpreter.Intepret(*charString, this);
	}
	else
		return status;
}

EStatusCode CFFFileInput::AddDependentGlyphs(UIntVector& ioSubsetGlyphIDs)
{
	EStatusCode status = PDFHummus::eSuccess;
	UIntSet glyphsSet;
	UIntVector::iterator it = ioSubsetGlyphIDs.begin();
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
		for(UIntSet::iterator itNewGlyphs = glyphsSet.begin(); itNewGlyphs != glyphsSet.end(); ++itNewGlyphs)
			ioSubsetGlyphIDs.push_back(*itNewGlyphs);

		std::sort(ioSubsetGlyphIDs.begin(), ioSubsetGlyphIDs.end());
	}
	return status;
}

EStatusCode CFFFileInput::CollectComponentGlyphs(unsigned int inGlyphID,
												 UIntSet& ioComponents,
												 bool& outFoundComponents,
												 unsigned int inDepth)
{
	outFoundComponents = false;

	if(inDepth > scMaxCompositeDepth)
	{
		// Cycles are blocked by the visited-set guard below, but a malicious
		// font can still build a deeply nested acyclic seac chain. Cap depth
		// to keep the call stack bounded.
		TRACE_LOG2("CFFFileInput::CollectComponentGlyphs, composite depth %u exceeds cap %u, refusing to recurse further.",
			inDepth, scMaxCompositeDepth);
		return PDFHummus::eSuccess;
	}

	CharString2Dependencies dependencies;
	EStatusCode status = CalculateDependenciesForCharIndex(0, (unsigned short)inGlyphID, dependencies);

	if(PDFHummus::eSuccess == status && dependencies.mCharCodes.size() != 0)
	{
		UShortSet::iterator it = dependencies.mCharCodes.begin();
		for(; it != dependencies.mCharCodes.end() && PDFHummus::eSuccess == status; ++it)
		{
			bool dummyFound;
			// Recurse only when the component is new to the set. A glyph
			// referencing itself or two glyphs referencing each other would
			// otherwise drive the call stack until it overflows. The set
			// doubles as the visited marker for cycle detection.
			if(ioComponents.insert(*it).second)
				status = CollectComponentGlyphs(*it, ioComponents, dummyFound, inDepth + 1);
		}
		outFoundComponents = true;
	}
	return status;
}

EStatusCode CFFFileInput::PrepareForGlyphIntepretation(	unsigned short inFontIndex,
			 											unsigned short inCharStringIndex)
{
	if(inFontIndex >= mFontsCount)
	{
		TRACE_LOG2("CFFFileInput::PrepareForGlyphIntepretation, inFontIndex = %d is invalid. there are %d fonts in the CFF segment",inFontIndex,mFontsCount);
		return PDFHummus::eFailure;
	}

	if(mCharStrings[inFontIndex].mCharStringsCount <= inCharStringIndex)
	{
		TRACE_LOG2("CFFFileInput::PrepareForGlyphIntepretation, inCharStringIndex = %d is invalid. there are %d charsringd in the CFF segment for the requested font",
																														inCharStringIndex,mCharStrings[inFontIndex].mCharStringsCount);
		return PDFHummus::eFailure;
	}

	if(2 == mCharStrings[inFontIndex].mCharStringsType)
	{
		if(mTopDictIndex[inFontIndex].mFDSelect) // CIDs have FDSelect
		{
			mCurrentLocalSubrs = mTopDictIndex[inFontIndex].mFDSelect[inCharStringIndex]->mPrivateDict.mLocalSubrs;
			mCurrentCharsetInfo = mTopDictIndex[inFontIndex].mCharSet;
			mCurrentDependencies = NULL;
		}
		else
		{
			mCurrentLocalSubrs = mPrivateDicts[inFontIndex].mLocalSubrs;
			mCurrentCharsetInfo = mTopDictIndex[inFontIndex].mCharSet;
			mCurrentDependencies = NULL;
		}
		return PDFHummus::eSuccess;
	}
	else
	{
		TRACE_LOG1("CFFFileInput::PrepareForGlyphIntepretation, unsupported charstring format = %d. only type 2 charstrings are supported",mCharStrings[inFontIndex].mCharStringsType);
		return PDFHummus::eFailure;
	}
}

CharString* CFFFileInput::GetGlyphCharString(unsigned short inFontIndex,
											 unsigned short inCharStringIndex)
{
	if(inFontIndex >= mFontsCount)
	{
		TRACE_LOG2("CFFFileInput::CalculateDependenciesForCharIndex, inFontIndex = %d is invalid. there are %d fonts in the CFF segment",inFontIndex,mFontsCount);
		return NULL;
	}

	if(mCharStrings[inFontIndex].mCharStringsCount <= inCharStringIndex)
	{
		TRACE_LOG2("CFFFileInput::CalculateDependenciesForCharIndex, inCharStringIndex = %d is invalid. there are %d charsringd in the CFF segment for the requested font",
																														inCharStringIndex,mCharStrings[inFontIndex].mCharStringsCount);
		return NULL;
	}

	return mCharStrings[inFontIndex].mCharStringsIndex + inCharStringIndex;
}


EStatusCode CFFFileInput::ReadCharString(	LongFilePositionType inCharStringStart,
											LongFilePositionType inCharStringEnd,
											Byte** outCharString)
{
	EStatusCode status = PDFHummus::eSuccess;
	*outCharString = NULL;

	// end must be >= start; otherwise the unsigned subtraction below underflows.
	if(inCharStringEnd < inCharStringStart)
	{
		TRACE_LOG2("CFFFileInput::ReadCharString, end (%lld) < start (%lld)",
			(long long)inCharStringEnd, (long long)inCharStringStart);
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(inCharStringStart);

	do
	{
		*outCharString = new Byte[(LongBufferSizeType)(inCharStringEnd - inCharStringStart)];

		status = mPrimitivesReader.Read(*outCharString,(LongBufferSizeType)(inCharStringEnd - inCharStringStart));
		if(status != PDFHummus::eSuccess)
			break;

	}while(false);

	if(status != PDFHummus::eSuccess && *outCharString)
	{
		delete[] *outCharString;
		*outCharString = NULL;
	}

	return status;
}

CharString* CFFFileInput::GetLocalSubr(long inSubrIndex)
{
	// locate local subr and return. also - push it to the dependendecy stack to start calculating dependencies for it
	// also - record dependency on this subr.
	long biasedIndex = GetBiasedIndex(mCurrentLocalSubrs->mCharStringsCount,inSubrIndex);

	if(biasedIndex >= 0 && biasedIndex < (long)mCurrentLocalSubrs->mCharStringsCount)
	{
		CharString* returnValue = mCurrentLocalSubrs->mCharStringsIndex + biasedIndex;
		if(mCurrentDependencies)
			mCurrentDependencies->mLocalSubrs.insert((unsigned short)biasedIndex);
		return returnValue;
	}
	else
		return NULL;
}

long CFFFileInput::GetBiasedIndex(unsigned short inSubroutineCollectionSize, long inSubroutineIndex)
{
	// Compute the bias + index sum in 64-bit so that extreme attacker-
	// controlled inSubroutineIndex values don't trigger signed-overflow UB
	// during the addition on platforms where long is 32 bits (LLP64, e.g.
	// Windows). Saturate to LONG_MIN / LONG_MAX on out-of-range — the
	// caller's range check will reject either sentinel just like any
	// other out-of-bounds index.
	int64_t bias;
	if(inSubroutineCollectionSize < 1240)
		bias = 107;
	else if(inSubroutineCollectionSize < 33900)
		bias = 1131;
	else
		bias = 32768;

	int64_t result = bias + (int64_t)inSubroutineIndex;
	if(result > (int64_t)LONG_MAX) return LONG_MAX;
	if(result < (int64_t)LONG_MIN) return LONG_MIN;
	return (long)result;
}

CharString* CFFFileInput::GetGlobalSubr(long inSubrIndex)
{
	long biasedIndex = GetBiasedIndex(mGlobalSubrs.mCharStringsCount,inSubrIndex);

	if(biasedIndex >= 0 && biasedIndex < (long)mGlobalSubrs.mCharStringsCount)
	{
		CharString* returnValue = mGlobalSubrs.mCharStringsIndex + biasedIndex;
		if(mCurrentDependencies)
			mCurrentDependencies->mGlobalSubrs.insert((unsigned short)biasedIndex);
		return returnValue;
	}
	else
		return NULL;
}

EStatusCode CFFFileInput::Type2Endchar(const CharStringOperandList& inOperandList)
{
	// i'm using EndChar here to check the depracated usage, which creates
	// dependency on another charachter. as for finalizing the intepretation, i don't
	// need to do anything here, and just make sure to pop the whole dependency stack
	// in the interpretation calling method.

	if(inOperandList.size() >= 4) // meaning it's got the depracated seac usage. 2 topmost charachters on the stack are charachter codes of off StandardEncoding
	{
		CharStringOperandList::const_reverse_iterator it = inOperandList.rbegin();
		// seac character codes index StandardEncoding (0..255). Validate each
		// operand as a finite double in [0, 255] *before* narrowing — casting
		// NaN / ±inf / out-of-long-range reals to an integer type is UB in
		// C++, so the bounds check has to be in the double domain. NaN fails
		// every comparison, so the negated form rejects it implicitly along
		// with values outside [0, 255].
		double codeAsDouble1 = it->IsInteger ? (double)it->IntegerValue : it->RealValue;
		++it;
		double codeAsDouble2 = it->IsInteger ? (double)it->IntegerValue : it->RealValue;

		if(!(codeAsDouble1 >= 0.0 && codeAsDouble1 <= 255.0) ||
		   !(codeAsDouble2 >= 0.0 && codeAsDouble2 <= 255.0))
		{
			TRACE_LOG2("CFFFileInput::Type2Endchar, seac character code out of [0,255]: %f %f", codeAsDouble1, codeAsDouble2);
			return PDFHummus::eFailure;
		}

		CharString* character1 = GetCharacterFromStandardEncoding((Byte)codeAsDouble1);
		CharString* character2 = GetCharacterFromStandardEncoding((Byte)codeAsDouble2);

		if(character1 && character2 && mCurrentDependencies)
		{
			mCurrentDependencies->mCharCodes.insert(character1->mIndex);
			mCurrentDependencies->mCharCodes.insert(character2->mIndex);
			return PDFHummus::eSuccess;
		}	
		else
			return PDFHummus::eFailure;
	}

	return PDFHummus::eSuccess;
}

CharString* CFFFileInput::GetCharacterFromStandardEncoding(Byte inCharacterCode)
{
	if(mCurrentCharsetInfo == NULL)
		return NULL;

	StandardEncoding standardEncoding;
	const char* glyphName = standardEncoding.GetEncodedGlyphName(inCharacterCode);
	CharPToUShortMap::iterator itStringToSID = 	mStringToSID.find(glyphName);

	if(itStringToSID != mStringToSID.end())
	{
		UShortToCharStringMap::iterator itSIDToGlyph = mCurrentCharsetInfo->mSIDToGlyphMap.find(itStringToSID->second);
		if(itSIDToGlyph != mCurrentCharsetInfo->mSIDToGlyphMap.end())
			return itSIDToGlyph->second;
		else
			return NULL;
	}
	else
		return NULL;
}

EStatusCode CFFFileInput::ReadCIDInformation()
{
	EStatusCode status = PDFHummus::eSuccess;

	for(unsigned long i=0; i < mFontsCount && (PDFHummus::eSuccess == status); ++i)
	{
		// CID font will be identified by the existance of the ROS entry
		if(mTopDictIndex[i].mTopDict.find(scROS) != mTopDictIndex[i].mTopDict.end())
		{
			status = ReadFDArray(i);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDArray for font index %d",i);
				break;
			}

			status = ReadFDSelect(i);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDSelect for font index %d",i);
				break;
			}

		}
	}

	return status;
}

EStatusCode CFFFileInput::ReadFDArray(unsigned short inFontIndex)
{
	LongFilePositionType fdArrayLocation = GetFDArrayPosition(inFontIndex);

	// supposed to get here only for CIDs. and they must have an FDArray...so if it doesn't - fail
	if(0 == fdArrayLocation)
		return PDFHummus::eFailure;

	mPrimitivesReader.SetOffset(fdArrayLocation);
	
	unsigned long* offsets;
	unsigned short dictionariesCount;
	unsigned long i;
	EStatusCode status = ReadIndexHeader(&offsets,dictionariesCount);
	
	do
	{
		if(status != PDFHummus::eSuccess)
			break;

		if(offsets[0] != 1)
			mPrimitivesReader.Skip(offsets[0] - 1);

		mTopDictIndex[inFontIndex].mFDArray = new FontDictInfo[dictionariesCount];
		mTopDictIndex[inFontIndex].mFDArrayCount = dictionariesCount;

		for(i = 0; i < dictionariesCount && (PDFHummus::eSuccess == status); ++i)
		{
			mTopDictIndex[inFontIndex].mFDArray[i].mFontDictStart = mPrimitivesReader.GetCurrentPosition();
			status = ReadDict(offsets[i+1] - offsets[i],mTopDictIndex[inFontIndex].mFDArray[i].mFontDict);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("CFFFileInput::ReadFDArray, failed to read FDArray");
				break;
			}
			mTopDictIndex[inFontIndex].mFDArray[i].mFontDictEnd = mPrimitivesReader.GetCurrentPosition();
		}

		// another loop for reading the privates [should be one per font dict]. make sure to get their font subrs reference right
		for(i = 0; i < dictionariesCount && (PDFHummus::eSuccess == status); ++i)
		{
			status = ReadPrivateDict(mTopDictIndex[inFontIndex].mFDArray[i].mFontDict,&(mTopDictIndex[inFontIndex].mFDArray[i].mPrivateDict));
			if(PDFHummus::eSuccess == status)
				status = ReadLocalSubrsForPrivateDict(&(mTopDictIndex[inFontIndex].mFDArray[i].mPrivateDict),(Byte)GetCharStringType(inFontIndex));
				
		}

	}while(false);

	delete[] offsets;
	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
	
}

static const unsigned short scFDArray = 0xC24;
LongFilePositionType CFFFileInput::GetFDArrayPosition(unsigned short inFontIndex)
{
	return GetSingleIntegerValue(inFontIndex,scFDArray,0);
}

EStatusCode CFFFileInput::ReadFDSelect(unsigned short inFontIndex)
{
	LongFilePositionType fdSelectLocation = GetFDSelectPosition(inFontIndex);
	unsigned short glyphCount = mCharStrings[inFontIndex].mCharStringsCount;
	unsigned short fdArrayCount = mTopDictIndex[inFontIndex].mFDArrayCount;
	EStatusCode status = PDFHummus::eSuccess;
	Byte format;

	// supposed to get here only for CIDs. and they must have an FDSelect...so if it doesn't - fail
	if(0 == fdSelectLocation)
		return PDFHummus::eFailure;

	mTopDictIndex[inFontIndex].mFDSelect = new FontDictInfo*[glyphCount];
	mPrimitivesReader.SetOffset(fdSelectLocation);

	mPrimitivesReader.ReadCard8(format);
	if(0 == format)
	{
		Byte fdIndex;

		for(unsigned long i=0; i < glyphCount && PDFHummus::eSuccess == status; ++i)
		{
			status = mPrimitivesReader.ReadCard8(fdIndex);
			if(status == PDFHummus::eSuccess)
			{
				if(fdIndex >= fdArrayCount)
				{
					TRACE_LOG2("CFFFileInput::ReadFDSelect, format 0 fdIndex %u out of range (fdArrayCount=%u)", fdIndex, fdArrayCount);
					return PDFHummus::eFailure;
				}
				mTopDictIndex[inFontIndex].mFDSelect[i] = mTopDictIndex[inFontIndex].mFDArray + fdIndex;
			}
		}
	}
	else // format 3
	{
		unsigned short rangesCount;
		unsigned short firstGlyphIndex;
		unsigned short nextRangeGlyphIndex;
		Byte fdIndex;

		status = mPrimitivesReader.ReadCard16(rangesCount);
		if(status == PDFHummus::eSuccess)
		{
			status = mPrimitivesReader.ReadCard16(firstGlyphIndex);
			if(status == PDFHummus::eSuccess && firstGlyphIndex != 0)
			{
				// Format 3 must cover every glyph; a non-zero starting index
				// would leave mFDSelect[0..firstGlyphIndex) uninitialized for
				// later glyph-interpretation lookups to dereference.
				TRACE_LOG1("CFFFileInput::ReadFDSelect, format 3 initial firstGlyphIndex %u != 0 leaves leading glyphs unassigned", firstGlyphIndex);
				return PDFHummus::eFailure;
			}
			for(unsigned long i=0; i < rangesCount && PDFHummus::eSuccess == status;++i)
			{
				mPrimitivesReader.ReadCard8(fdIndex);
				mPrimitivesReader.ReadCard16(nextRangeGlyphIndex);
				status = mPrimitivesReader.GetInternalState();
				if(status != PDFHummus::eSuccess)
					break;
				if(fdIndex >= fdArrayCount)
				{
					TRACE_LOG2("CFFFileInput::ReadFDSelect, format 3 fdIndex %u out of range (fdArrayCount=%u)", fdIndex, fdArrayCount);
					return PDFHummus::eFailure;
				}
				if(nextRangeGlyphIndex > glyphCount || nextRangeGlyphIndex <= firstGlyphIndex)
				{
					TRACE_LOG2("CFFFileInput::ReadFDSelect, format 3 nextRangeGlyphIndex %u outside (firstGlyphIndex, glyphCount=%u]", nextRangeGlyphIndex, glyphCount);
					return PDFHummus::eFailure;
				}
				for(unsigned short j=firstGlyphIndex; j < nextRangeGlyphIndex;++j)
					mTopDictIndex[inFontIndex].mFDSelect[j] =
						mTopDictIndex[inFontIndex].mFDArray + fdIndex;
				firstGlyphIndex = nextRangeGlyphIndex;
			}
			if(status == PDFHummus::eSuccess && firstGlyphIndex != glyphCount)
			{
				// After the loop firstGlyphIndex holds the final sentinel
				// (or the initial value if rangesCount was 0). It must equal
				// glyphCount; otherwise mFDSelect[firstGlyphIndex..glyphCount)
				// stays uninitialized.
				TRACE_LOG2("CFFFileInput::ReadFDSelect, format 3 final sentinel %u != glyphCount %u leaves trailing glyphs unassigned", firstGlyphIndex, glyphCount);
				return PDFHummus::eFailure;
			}
		}
	}

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

static const unsigned short scFDSelect = 0xC25;
LongFilePositionType CFFFileInput::GetFDSelectPosition(unsigned short inFontIndex)
{
	return GetSingleIntegerValue(inFontIndex,scFDSelect,0);
}


EStatusCode CFFFileInput::ReadCFFFileByIndexOrName(IByteReaderWithPosition* inCFFFile,const std::string& inFontName,unsigned short inFontIndex)
{
	// read either by font name or index. read by font name if not empty, otherwise by index
	EStatusCode status;
	unsigned short fontIndex;

	do
	{
		FreeData();
		mPrimitivesReader.SetStream(inCFFFile);
		mCFFOffset = inCFFFile->GetCurrentPosition();

		status = ReadHeader();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile, Failed to read header");
			break;
		}

		// skip to name
		if(mHeader.hdrSize > 4)
			mPrimitivesReader.Skip(mHeader.hdrSize - 4);

		status = ReadNameIndex();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile Failed to read name");
			break;
		}

		if(inFontName.size() > 0)
		{
			BoolAndUShort fontIndexResult = GetIndexForFontName(inFontName);
			if(fontIndexResult.first)
			{
				fontIndex = fontIndexResult.second;
			}
			else
			{
				TRACE_LOG1("CFFFileInput::ReadCFFFile, font name %s was not found in font stream",inFontName.c_str());
				break;
			}
		}
		else
		{
			// make sure index is available in the fontsRange
			if(inFontIndex >= mFontsCount)
			{
				TRACE_LOG2("CFFFileInput::ReadCFFFile, input index (%d) is larger than the maximum possible index (%d)",inFontIndex,mFontsCount-1);
				break;
			}
			fontIndex = inFontIndex;
		}		

		status = ReadTopDictIndex(fontIndex);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
			break;
		}

		status = ReadStringIndex();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
			break;
		}

		status = ReadGlobalSubrs();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read global subrs");
			break;
		}

		status = ReadCharStrings(fontIndex);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
			break;
		}

		status = ReadPrivateDicts(fontIndex);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
			break;
		}

		status = ReadLocalSubrs(fontIndex);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read local subrs");
			break;
		}

		status = ReadCharsets(fontIndex);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read char set");
			break;
		}

		status = ReadEncodings(fontIndex);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read encodings");
			break;
		}
		
		status = ReadCIDInformation();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CFFFileInput::ReadCFFFile failed to read CID Information");
			break;
		}

	}while(false);
	return status;
}

EStatusCode CFFFileInput::ReadTopDictIndex(unsigned short inFontIndex)
{
	mTopDictIndexPosition = mPrimitivesReader.GetCurrentPosition();

	// will read only what's necessary for the font index, nulling items in the path
	
	unsigned long* offsets;
	unsigned short dictionariesCount;
	EStatusCode status = ReadIndexHeader(&offsets,dictionariesCount);
	
	do
	{
		if(status != PDFHummus::eSuccess)
			break;

		// allocate all, but read just the required font
		mTopDictIndex = new TopDictInfo[dictionariesCount];

		if(offsets[inFontIndex] != 1)
			mPrimitivesReader.Skip(offsets[inFontIndex] - 1);
		status = ReadDict(offsets[inFontIndex+1] - offsets[inFontIndex],mTopDictIndex[inFontIndex].mTopDict);
	}while(false);

	delete[] offsets;
	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadCharStrings(unsigned short inFontIndex)
{
	// allocate all, but read just the required one
	mCharStrings = new CharStrings[mFontsCount];
	EStatusCode status = PDFHummus::eSuccess;

	LongFilePositionType  charStringsPosition = GetCharStringsPosition(inFontIndex);	
	mCharStrings[inFontIndex].mCharStringsType = (Byte)GetCharStringType(inFontIndex);
	if(charStringsPosition != 0)
	{
		mPrimitivesReader.SetOffset(charStringsPosition);
		status = ReadSubrsFromIndex(mCharStrings[inFontIndex].mCharStringsCount,
									&(mCharStrings[inFontIndex].mCharStringsIndex));
	}

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadPrivateDicts(unsigned short inFontIndex)
{
	// allocate all (to maintain indexing...whatever), but read just the relevant font dict
	mPrivateDicts = new PrivateDictInfo[mFontsCount];
	EStatusCode status = ReadPrivateDict(mTopDictIndex[inFontIndex].mTopDict,mPrivateDicts + inFontIndex);

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();	
}

EStatusCode CFFFileInput::ReadLocalSubrs(unsigned short inFontIndex)
{
	// read just the localsubrs of the font private dict
	return ReadLocalSubrsForPrivateDict(mPrivateDicts+inFontIndex,(Byte)GetCharStringType(inFontIndex));
}

EStatusCode CFFFileInput::ReadCharsets(unsigned short inFontIndex)
{
	// read all charsets
	EStatusCode status = PDFHummus::eSuccess;

	LongFilePositionType charsetPosition = GetCharsetPosition(inFontIndex);
	CharSetInfo* charSet = new CharSetInfo();
	bool isCID = mTopDictIndex[inFontIndex].mTopDict.find(scROS) != mTopDictIndex[inFontIndex].mTopDict.end();
	if(charsetPosition <= 2)
	{
		charSet->mType = (ECharSetType)charsetPosition;
		charSet->mSIDs = NULL;
		if(!isCID) // collect SID->Glyph map only if not CID, in which case SIDs are CIDs...and what i'm using the map for is irrelevant
			SetupSIDToGlyphMapWithStandard(scDefaultCharsets[charsetPosition],
										   scDefaultCharsetsSizes[charsetPosition],
										   charSet->mSIDToGlyphMap,
										   mCharStrings[inFontIndex]);
	}
	else
	{
		Byte charsetFormat;
		charSet->mType = eCharSetCustom;
		mPrimitivesReader.SetOffset(charsetPosition);
		mPrimitivesReader.ReadCard8(charsetFormat);

		if(0 == charsetFormat)
			status = ReadFormat0Charset(isCID, charSet->mSIDToGlyphMap,
										&charSet->mSIDs,
										mCharStrings[inFontIndex]);
		else if(1 == charsetFormat)
			status = ReadFormat1Charset(isCID, charSet->mSIDToGlyphMap,
										&charSet->mSIDs,
										mCharStrings[inFontIndex]);
		else // 2 charset format
			status = ReadFormat2Charset(isCID, charSet->mSIDToGlyphMap,
										&charSet->mSIDs,
										mCharStrings[inFontIndex]);
	}
	mCharSets.push_back(charSet);
	mTopDictIndex[inFontIndex].mCharSet = charSet;

	if(status != PDFHummus::eSuccess)
		return status;
	else
		return mPrimitivesReader.GetInternalState();
}


EStatusCode CFFFileInput::ReadEncodings(unsigned short inFontIndex)
{
	LongFilePositionType encodingPosition = GetEncodingPosition(inFontIndex);
	EncodingsInfo* encoding = new EncodingsInfo();

	// Push first so mEncodings owns the buffer regardless of outcome — the
	// destructor walks it for cleanup. Only assign mTopDictIndex on success
	// to avoid pinning a partially-initialised encoding as the font's.
	EStatusCode status = ReadEncoding(encoding, encodingPosition);
	mEncodings.push_back(encoding);
	if(status != PDFHummus::eSuccess)
	{
		TRACE_LOG1("CFFFileInput::ReadEncodings, failed to read encoding for font index %u", inFontIndex);
		return status;
	}
	mTopDictIndex[inFontIndex].mEncoding = encoding;

	return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadCIDInformation(unsigned short inFontIndex)
{
	EStatusCode status = PDFHummus::eSuccess;

	// CID font will be identified by the existance of the ROS entry
	if(mTopDictIndex[inFontIndex].mTopDict.find(scROS) != mTopDictIndex[inFontIndex].mTopDict.end())
	{
		do
		{
			status = ReadFDArray(inFontIndex);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDArray for font index %d",inFontIndex);
				break;
			}

			status = ReadFDSelect(inFontIndex);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDSelect for font index %d",inFontIndex);
				break;
			}
		}while(false);
	}
	return status;
}


EStatusCode CFFFileInput::ReadCFFFile(IByteReaderWithPosition* inCFFFile,unsigned short inFontIndex)
{
	return ReadCFFFileByIndexOrName(inCFFFile,"",inFontIndex);
}

EStatusCode CFFFileInput::ReadCFFFile(IByteReaderWithPosition* inCFFFile,const std::string& inFontName)
{
	return ReadCFFFileByIndexOrName(inCFFFile,inFontName,0);
}


unsigned short CFFFileInput::GetFontsCount(unsigned short inFontIndex)
{
	return mFontsCount;
}

unsigned short CFFFileInput::GetCharStringsCount(unsigned short inFontIndex)
{
	if(inFontIndex >= mFontsCount)
		return 0;
	else
		return mCharStrings[inFontIndex].mCharStringsCount;
}

std::string CFFFileInput::GetGlyphName(unsigned short inFontIndex,unsigned short inGlyphIndex)
{
	return GetStringForSID(GetGlyphSID(inFontIndex,inGlyphIndex));
}

unsigned short CFFFileInput::GetGlyphSID(unsigned short inFontIndex,unsigned short inGlyphIndex)
{
	if(inFontIndex >= mFontsCount || inGlyphIndex >= mCharStrings[inFontIndex].mCharStringsCount)
	{
		return 0;
	}
	else
	{
		unsigned short sid;
		if(0 == inGlyphIndex)
		{
			sid = 0;
		}
		else
		{
			if(eCharSetCustom == mTopDictIndex[inFontIndex].mCharSet->mType)
			{
				sid = mTopDictIndex[inFontIndex].mCharSet->mSIDs[inGlyphIndex];
			}
			else
			{
				// SID 0 is omitted for the default charsets
				sid = scDefaultCharsets[(Byte)mTopDictIndex[inFontIndex].mCharSet->mType][inGlyphIndex-1];
			}
		}
		return sid;
	}
}

