/*
   Source File : Type1ToCFFEmbeddedFontWriter.h


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
#pragma once
#include "EPDFStatusCode.h"
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

	EPDFStatusCode WriteEmbeddedFont(	FreeTypeFaceWrapper& inFontInfo,
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

	EPDFStatusCode CreateCFFSubset(	
								FreeTypeFaceWrapper& inFontInfo,
								const UIntVector& inSubsetGlyphIDs,
								const string& inSubsetFontName,
								bool& outNotEmbedded,
								MyStringBuf& outFontProgram);
	EPDFStatusCode AddDependentGlyphs(StringVector& ioSubsetGlyphIDs);
	EPDFStatusCode AddComponentGlyphs(const string& inGlyphID,StringSet& ioComponents,bool &outFoundComponents);
	EPDFStatusCode WriteCFFHeader();
	EPDFStatusCode WriteName(const string& inSubsetFontName);
	Byte GetMostCompressedOffsetSize(unsigned long inOffset);
	EPDFStatusCode WriteTopIndex();
	EPDFStatusCode WriteTopDictSegment(MyStringBuf& ioTopDictSegment);
	unsigned short AddStringToStringsArray(const string& inString);
	BoolAndUShort FindStandardString(const string& inStringToFind);
	void AddStringOperandIfNotEmpty(CFFPrimitiveWriter& inWriter,const string& inString,unsigned short inOperator);
	void AddNumberOperandIfNotDefault(CFFPrimitiveWriter& inWriter,int inOperand,unsigned short inOperator,int inDefault);
	void AddNumberOperandIfNotDefault(CFFPrimitiveWriter& inWriter,double inOperand,unsigned short inOperator,double inDefault);
	void AddDeltaVectorIfNotEmpty(CFFPrimitiveWriter& inWriter,const vector<int>& inArray,unsigned short inOperator);
	void AddDeltaVectorIfNotEmpty(CFFPrimitiveWriter& inWriter,const vector<double>& inArray,unsigned short inOperator);
	EPDFStatusCode WriteStringIndex();
	EPDFStatusCode WriteGlobalSubrsIndex();
	EPDFStatusCode WriteEncodings(const StringVector& inSubsetGlyphIDs);
	void FreeTemporaryStructs();
	void PrepareCharSetArray(const StringVector& inSubsetGlyphIDs);
	EPDFStatusCode WriteCharsets(const StringVector& inSubsetGlyphIDs);
	EPDFStatusCode WriteCharStrings(const StringVector& inSubsetGlyphIDs);
	EPDFStatusCode WritePrivateDictionary();
	EPDFStatusCode UpdateIndexesAtTopDict();
	void TranslateFromFreeTypeToType1(FreeTypeFaceWrapper& inFontInfo,
									  const UIntVector& inSubsetGlyphIDs,
									  StringVector& outGlyphNames);

};
