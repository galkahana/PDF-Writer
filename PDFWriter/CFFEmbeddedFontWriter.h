#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "OpenTypeFileInput.h"
#include "MyStringBuf.h"
#include "InputFile.h"
#include "CFFPrimitiveWriter.h"
#include "OutputStringBufferStream.h"
#include "IOBasicTypes.h"

#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;
using namespace IOBasicTypes;

typedef vector<unsigned int> UIntVector;
typedef set<unsigned int> UIntSet;
typedef vector<unsigned short> UShortVector;
typedef map<FontDictInfo*,Byte> FontDictInfoToByteMap;


class FreeTypeFaceWrapper;
class ObjectsContext;

class CFFEmbeddedFontWriter
{
public:
	CFFEmbeddedFontWriter(void);
	~CFFEmbeddedFontWriter(void);

	EStatusCode WriteEmbeddedFont(	FreeTypeFaceWrapper& inFontInfo,
									const UIntVector& inSubsetGlyphIDs,
									const string& inFontFile3SubType,
									const string& inSubsetFontName,
									ObjectsContext* inObjectsContext,
									ObjectIDType& outEmbeddedFontObjectID);

	// the optional inCIDMapping parameter provides a vector ordered in the same
	// way as the glyph IDs. for each position in the CID mapping vector there's the matching CID
	// for the GID in the same position in the subset glyph IDs.
	// use it when the CFF origin is from a subset font, and the GID->CID mapping is not simply
	// identity
	EStatusCode WriteEmbeddedFont(	FreeTypeFaceWrapper& inFontInfo,
									const UIntVector& inSubsetGlyphIDs,
									const string& inFontFile3SubType,
									const string& inSubsetFontName,
									ObjectsContext* inObjectsContext,
									UShortVector* inCIDMapping,
									ObjectIDType& outEmbeddedFontObjectID);


private:
	OpenTypeFileInput mOpenTypeInput;
	InputFile mOpenTypeFile;
	CFFPrimitiveWriter mPrimitivesWriter;
	OutputStringBufferStream mFontFileStream;
	bool mIsCID;
	string mOptionalEmbeddedPostscript;

	// placeholders positions
	LongFilePositionType mCharsetPlaceHolderPosition;
	LongFilePositionType mEncodingPlaceHolderPosition;
	LongFilePositionType mCharstringsPlaceHolderPosition;
	LongFilePositionType mPrivatePlaceHolderPosition;
	LongFilePositionType mFDArrayPlaceHolderPosition;
	LongFilePositionType mFDSelectPlaceHolderPosition;
	
	LongFilePositionType mEncodingPosition;
	LongFilePositionType mCharsetPosition;
	LongFilePositionType mCharStringPosition;
	LongFilePositionType mPrivatePosition;
	LongFilePositionType mPrivateSize;
	LongFilePositionType mFDArrayPosition;
	LongFilePositionType mFDSelectPosition;

	EStatusCode CreateCFFSubset(	
					FreeTypeFaceWrapper& inFontInfo,
					const UIntVector& inSubsetGlyphIDs,
					UShortVector* inCIDMapping,
					const string& inSubsetFontName,
					bool& outNotEmbedded,
					MyStringBuf& outFontProgram);
	EStatusCode AddDependentGlyphs(UIntVector& ioSubsetGlyphIDs);
	EStatusCode AddComponentGlyphs(unsigned int inGlyphID,UIntSet& ioComponents,bool &outFoundComponents);
	EStatusCode WriteCFFHeader();
	EStatusCode WriteName(const string& inSubsetFontName);
	EStatusCode WriteTopIndex();
	Byte GetMostCompressedOffsetSize(unsigned long inOffset);
	EStatusCode WriteTopDictSegment(MyStringBuf& ioTopDictSegment);
	EStatusCode WriteStringIndex();
	EStatusCode WriteGlobalSubrsIndex();
	EStatusCode WriteEncodings(const UIntVector& inSubsetGlyphIDs);
	EStatusCode WriteCharsets(const UIntVector& inSubsetGlyphIDs,UShortVector* inCIDMapping);
	EStatusCode WriteCharStrings(const UIntVector& inSubsetGlyphIDs);
	EStatusCode WritePrivateDictionary();

	EStatusCode WriteFDArray(const UIntVector& inSubsetGlyphIDs,const FontDictInfoToByteMap& inNewFontDictsIndexes);
	EStatusCode WriteFDSelect(const UIntVector& inSubsetGlyphIDs,const FontDictInfoToByteMap& inNewFontDictsIndexes);
	EStatusCode WritePrivateDictionaryBody(	const PrivateDictInfo& inPrivateDictionary,
											LongFilePositionType& outWriteSize,
											LongFilePositionType& outWritePosition);
	EStatusCode UpdateIndexesAtTopDict();

	void DetermineFDArrayIndexes(const UIntVector& inSubsetGlyphIDs,FontDictInfoToByteMap& outNewFontDictsIndexes);
	
};
