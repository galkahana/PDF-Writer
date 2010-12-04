#pragma once

#include "EStatusCode.h"
#include "IByteReaderWithPosition.h"
#include "CFFPrimitiveReader.h"
#include "IType2InterpreterImplementation.h"

#include <string>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace std;

struct CFFHeader
{
	Byte major;
	Byte minor;
	Byte hdrSize;
	Byte offSize;
};

typedef list<string> StringList;
typedef map<string,unsigned short> StringToUShort;
typedef pair<bool,unsigned short> BoolAndUShort;

typedef list<CharString*> CharStringList;

typedef CharString* CharStringsIndex;

// this time it's the font charstrings
struct CharStrings
{
	CharStrings(){mCharStringsIndex = NULL; mCharStringsType = 0; mCharStringsCount = 0;}

	Byte mCharStringsType;
	unsigned short mCharStringsCount;
	CharStringsIndex mCharStringsIndex;
};

enum ECharSetType
{
	eCharSetISOAdobe = 0,
	eCharSetExpert,
	eCharSetExpertSubset,
	eCharSetCustom
};

typedef map<unsigned short,CharString*> UShortToCharStringMap;

struct CharSetInfo
{	
	CharSetInfo() {mSIDs = NULL;}

	ECharSetType mType;
	UShortToCharStringMap mSIDToGlyphMap;
	unsigned short* mSIDs; // count is like glyphs count
};

typedef vector<CharSetInfo*> CharSetInfoVector;


enum EEncodingType
{
	eEncodingStandard = 0,
	eEncodingExpert,
	eEncodingCustom
};

typedef list<Byte> ByteList;
typedef map<unsigned short,ByteList> UShortToByteList;

typedef pair<Byte,unsigned short> ByteAndUShort;

struct EncodingsInfo
{
	EncodingsInfo() {mEncoding = NULL;}

	LongFilePositionType mEncodingStart;
	LongFilePositionType mEncodingEnd;

	EEncodingType mType;
	Byte mEncodingsCount;
	Byte* mEncoding;
	UShortToByteList mSupplements;
	
};

struct PrivateDictInfo
{
	PrivateDictInfo() {mPrivateDictStart=0;mPrivateDictEnd=0;mLocalSubrs=NULL;}

	LongFilePositionType mPrivateDictStart;
	LongFilePositionType mPrivateDictEnd;
	UShortToDictOperandListMap mPrivateDict;
	CharStrings* mLocalSubrs;

};

typedef map<LongFilePositionType,CharStrings*> LongFilePositionTypeToCharStringsMap;

struct FontDictInfo
{
	LongFilePositionType mFontDictStart;
	LongFilePositionType mFontDictEnd;
	UShortToDictOperandListMap mFontDict;
	PrivateDictInfo mPrivateDict;
};

struct TopDictInfo
{
	TopDictInfo() {
					mFDArray = NULL;
					mFDSelect = NULL;
					mCharSet = NULL;
					mEncoding = NULL;
				}

	UShortToDictOperandListMap mTopDict;
	CharSetInfo* mCharSet;
	EncodingsInfo* mEncoding;
	FontDictInfo* mFDArray;
	FontDictInfo** mFDSelect; // size is like glyphsize. each cell references the relevant FontDict
};

typedef vector<EncodingsInfo*> EncodingsInfoVector;




class StringLess : public binary_function<const char*,const char*,bool>
{
public:
	bool operator( ) (const char* left, 
						const char* right ) const
	{
		return strcmp(left,right) < 0;
	}
};

typedef set<unsigned short> UShortSet;

struct CharString2Dependencies
{
	UShortSet mCharCodes; // from seac-like endchar operator
	UShortSet mGlobalSubrs; // from callgsubr
	UShortSet mLocalSubrs; // from callsubr
};

typedef map<const char*,unsigned short,StringLess> CharPToUShortMap;

class CFFFileInput : public Type2InterpreterImplementationAdapter
{
public:
	CFFFileInput(void);
	~CFFFileInput(void);

	// parses the whole CFF file, with all contained fonts
	EStatusCode ReadCFFFile(IByteReaderWithPosition* inCFFFile);
	// parses the CFF file just for the particular font according to index. Index should be 
	// according to how it appears in the CFF
	EStatusCode ReadCFFFile(IByteReaderWithPosition* inCFFFile,unsigned short inFontIndex);
	// parses the CFF file just for the particular named font
	EStatusCode ReadCFFFile(IByteReaderWithPosition* inCFFFile,const string& inFontName);

	// call only <i> after </i> calling the read method...got it?
	// calculate dependencies for a given charstring [it can be char, gsubr or localsubr].
	// the function is recursive, so that if the charstring has dependencies, then by the end of the
	// process their dependencies will be calculated too.
	// apart from the charstring you need to pass also the relevant localsubrs for this font, in case subrs
	// will be required. also the relevnat charstring and charset array should be passed in case
	// the seac flavor of endchar will be encountered.
	EStatusCode CalculateDependenciesForCharIndex(unsigned short inFontIndex,
												  unsigned short inCharStringIndex,
												  CharString2Dependencies& ioDependenciesInfo);

	unsigned short GetFontsCount(unsigned short inFontIndex);
	unsigned short GetCharStringsCount(unsigned short inFontIndex);
	string GetGlyphName(unsigned short inFontIndex,unsigned short inGlyphIndex);
	unsigned short GetGlyphSID(unsigned short inFontIndex,unsigned short inGlyphIndex);
	CharString* GetGlyphCharString(unsigned short inFontIndex,
				 				   unsigned short inCharStringIndex);

	// use this wonderful little fellow when interpreting a charstring with
	// CharStringType2Intepreter. This will set the CFFFileInput with 
	// the right items so that later you can use the IType2InterpreterImplementation
	// implementation here and avoid having to mess with passing gsubrs, lsubrs etc.
	// when interpreting just call these methods instead of yours to perform the relevant actions
	EStatusCode PrepareForGlyphIntepretation(unsigned short inFontIndex,
				 							unsigned short inCharStringIndex);

	void Reset();


	// IType2InterpreterImplementation overrides
	virtual EStatusCode ReadCharString(LongFilePositionType inCharStringStart,
							   LongFilePositionType inCharStringEnd,
							   Byte** outCharString);
	virtual CharString* GetLocalSubr(long inSubrIndex); 
	virtual CharString* GetGlobalSubr(long inSubrIndex);
	virtual EStatusCode Type2Endchar(const CharStringOperandList& inOperandList);


	// publicly available constructs
	
	// mCFFOffset should be added to any position here when referring to the beginning if the file containing this
	// segment. for instance, cff could be part of an OTF file definition, in which case the position is not 0.
	LongFilePositionType mCFFOffset;

	CFFHeader mHeader;
	unsigned short mFontsCount;
	StringList mName;
	TopDictInfo* mTopDictIndex; // count is same as fonts count
	char** mStrings;
	unsigned short mStringsCount;
	LongFilePositionType mStringIndexPosition;
	LongFilePositionType mGlobalSubrsPosition;
	PrivateDictInfo* mPrivateDicts; // private dicts are the same as fonts count. refers to the topdict related private dics, not to the FontDicts scenarios in CID.

private:
	CharStrings mGlobalSubrs;
	CharStrings* mCharStrings; // count is same as fonts count
	LongFilePositionTypeToCharStringsMap mLocalSubrs; // count is NOT the same as fonts count [some may be shared, plus there might be more because of CID usage]
	CharSetInfoVector mCharSets;// count is NOT the same as fonts count [some charsets may be shared]. consult the top dict charset pointer for the right charset
	EncodingsInfoVector mEncodings; // count is NOT the same as fonts count [some encodinds may be shared].

	CFFPrimitiveReader mPrimitivesReader;
	StringToUShort mNameToIndex;
	LongFilePositionType mNameIndexPosition;
	LongFilePositionType mTopDictIndexPosition;
	CharPToUShortMap mStringToSID;

	// for dependencies calculations using glyph interpretations. state.
	CharString2Dependencies* mCurrentDependencies;
	CharStrings* mCurrentLocalSubrs;
	CharStringList mAdditionalGlyphs;
	CharSetInfo* mCurrentCharsetInfo;

	string GetStringForSID(unsigned short inSID);	
	EStatusCode ReadHeader();
	EStatusCode ReadNameIndex();
	EStatusCode ReadIndexHeader(unsigned long** outOffsets,unsigned short& outItemsCount);
	EStatusCode ReadTopDictIndex();
	EStatusCode ReadDict(unsigned long inReadAmount,UShortToDictOperandListMap& outDict);
	EStatusCode ReadStringIndex();
	EStatusCode ReadGlobalSubrs();
	EStatusCode ReadCharStrings();
	EStatusCode ReadCharsets();
	EStatusCode ReadEncodings();
	void FreeData();
	LongFilePositionType GetCharStringsPosition(unsigned short inFontIndex);
	long GetSingleIntegerValue(unsigned short inFontIndex,unsigned short inKey,long inDefault);
	EStatusCode ReadSubrsFromIndex(unsigned short& outSubrsCount,CharStringsIndex* outSubrsIndex);
	long GetCharStringType(unsigned short inFontIndex);
	EStatusCode ReadPrivateDicts();
	EStatusCode ReadLocalSubrs();
	long GetSingleIntegerValueFromDict(const UShortToDictOperandListMap& inDict,
												 unsigned short inKey,
												 long inDefault);
	LongFilePositionType GetCharsetPosition(unsigned short inFontIndex);
	LongFilePositionType GetEncodingPosition(unsigned short inFontIndex);
	unsigned short GetBiasedIndex(unsigned short inSubroutineCollectionSize, long inSubroutineIndex);
	EStatusCode ReadFormat0Charset(bool inIsCID, UShortToCharStringMap& ioCharMap,unsigned short** inSIDArray,const CharStrings& inCharStrings);
	EStatusCode ReadFormat1Charset(bool inIsCID,UShortToCharStringMap& ioCharMap,unsigned short** inSIDArray,const CharStrings& inCharStrings);
	EStatusCode ReadFormat2Charset(bool inIsCID,UShortToCharStringMap& ioCharMap,unsigned short** inSIDArray,const CharStrings& inCharStrings);
	void SetupSIDToGlyphMapWithStandard(const unsigned short* inStandardCharSet,
										unsigned short inStandardCharSetLength,
										UShortToCharStringMap& ioCharMap,
										const CharStrings& inCharStrings);
	CharString* GetCharacterFromStandardEncoding(Byte inCharacterCode);
	EStatusCode ReadCIDInformation();
	EStatusCode ReadFDArray(unsigned short inFontIndex);
	EStatusCode ReadFDSelect(unsigned short inFontIndex);
	LongFilePositionType GetFDArrayPosition(unsigned short inFontIndex);
	EStatusCode ReadPrivateDict(const UShortToDictOperandListMap& inReferencingDict,PrivateDictInfo* outPrivateDict);
	EStatusCode ReadLocalSubrsForPrivateDict(PrivateDictInfo* inPrivateDict,Byte inCharStringType);
	LongFilePositionType GetFDSelectPosition(unsigned short inFontIndex);
	BoolAndUShort GetIndexForFontName(const string& inFontName);
	EStatusCode ReadTopDictIndex(unsigned short inFontIndex);
	EStatusCode ReadCharStrings(unsigned short inFontIndex);
	EStatusCode ReadPrivateDicts(unsigned short inFontIndex);
	EStatusCode ReadLocalSubrs(unsigned short inFontIndex);
	EStatusCode ReadCharsets(unsigned short inFontIndex);
	void ReadEncoding(EncodingsInfo* inEncoding,LongFilePositionType inEncodingPosition);
	EStatusCode ReadEncodings(unsigned short inFontIndex);
	EStatusCode ReadCIDInformation(unsigned short inFontIndex);
	EStatusCode ReadCFFFileByIndexOrName(IByteReaderWithPosition* inCFFFile,const string& inFontName,unsigned short inFontIndex);
};

