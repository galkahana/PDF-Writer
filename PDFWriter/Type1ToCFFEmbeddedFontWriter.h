#pragma once
#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "Type1Input.h"
#include "InputFile.h"
#include "CFFPrimitiveWriter.h"
#include "OutputStringBufferStream.h"
#include "MyStringBuf.h"


#include <vector>
#include <string>
#include <set>
#include <map>
#include <utility>

using namespace std;
using namespace IOBasicTypes;

typedef vector<unsigned int> UIntVector;
typedef set<unsigned int> UIntSet;
typedef vector<string> StringVector;
typedef map<string,unsigned short> StringToUShortMap;
typedef pair<bool,unsigned short> BoolAndUShort;
typedef set<string> StringSet;

class FreeTypeFaceWrapper;
class ObjectsContext;

class Type1ToCFFEmbeddedFontWriter
{
public:
	Type1ToCFFEmbeddedFontWriter(void);
	~Type1ToCFFEmbeddedFontWriter(void);

	EStatusCode WriteEmbeddedFont(	FreeTypeFaceWrapper& inFontInfo,
									const UIntVector& inSubsetGlyphIDs,
									const string& inFontFile3SubType,
									const string& inSubsetFontName,
									ObjectsContext* inObjectsContext,
									ObjectIDType& outEmbeddedFontObjectID);

private:
	Type1Input mType1Input;
	InputFile mType1File;
	CFFPrimitiveWriter mPrimitivesWriter;
	OutputStringBufferStream mFontFileStream;
	StringVector mStrings;
	StringToUShortMap mNonStandardStringToIndex;
	unsigned short* mCharset;

	// placeholders positions
	LongFilePositionType mCharsetPlaceHolderPosition;
	LongFilePositionType mEncodingPlaceHolderPosition;
	LongFilePositionType mCharstringsPlaceHolderPosition;
	LongFilePositionType mPrivatePlaceHolderPosition;


	LongFilePositionType mEncodingPosition;
	LongFilePositionType mCharsetPosition;
	LongFilePositionType mCharStringPosition;
	LongFilePositionType mPrivateSize;
	LongFilePositionType mPrivatePosition;

	EStatusCode CreateCFFSubset(	
								FreeTypeFaceWrapper& inFontInfo,
								const UIntVector& inSubsetGlyphIDs,
								const string& inSubsetFontName,
								bool& outNotEmbedded,
								MyStringBuf& outFontProgram);
	EStatusCode AddDependentGlyphs(StringVector& ioSubsetGlyphIDs);
	EStatusCode AddComponentGlyphs(const string& inGlyphID,StringSet& ioComponents,bool &outFoundComponents);
	EStatusCode WriteCFFHeader();
	EStatusCode WriteName(const string& inSubsetFontName);
	Byte GetMostCompressedOffsetSize(unsigned long inOffset);
	EStatusCode WriteTopIndex();
	EStatusCode WriteTopDictSegment(MyStringBuf& ioTopDictSegment);
	unsigned short AddStringToStringsArray(const string& inString);
	BoolAndUShort FindStandardString(const string& inStringToFind);
	void AddStringOperandIfNotEmpty(CFFPrimitiveWriter& inWriter,const string& inString,unsigned short inOperator);
	void AddNumberOperandIfNotDefault(CFFPrimitiveWriter& inWriter,int inOperand,unsigned short inOperator,int inDefault);
	void AddNumberOperandIfNotDefault(CFFPrimitiveWriter& inWriter,double inOperand,unsigned short inOperator,double inDefault);
	void AddDeltaVectorIfNotEmpty(CFFPrimitiveWriter& inWriter,const vector<int>& inArray,unsigned short inOperator);
	void AddDeltaVectorIfNotEmpty(CFFPrimitiveWriter& inWriter,const vector<double>& inArray,unsigned short inOperator);
	EStatusCode WriteStringIndex();
	EStatusCode WriteGlobalSubrsIndex();
	EStatusCode WriteEncodings(const StringVector& inSubsetGlyphIDs);
	void FreeTemporaryStructs();
	void PrepareCharSetArray(const StringVector& inSubsetGlyphIDs);
	EStatusCode WriteCharsets(const StringVector& inSubsetGlyphIDs);
	EStatusCode WriteCharStrings(const StringVector& inSubsetGlyphIDs);
	EStatusCode WritePrivateDictionary();
	EStatusCode UpdateIndexesAtTopDict();
	void TranslateFromFreeTypeToType1(FreeTypeFaceWrapper& inFontInfo,
									  const UIntVector& inSubsetGlyphIDs,
									  StringVector& outGlyphNames);

};
