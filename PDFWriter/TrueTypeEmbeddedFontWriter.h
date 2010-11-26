#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "OpenTypeFileInput.h"
#include "OutputStringBufferStream.h"
#include "InputFile.h"
#include "TrueTypePrimitiveWriter.h"
#include "InputStringBufferStream.h"
#include "OpenTypePrimitiveReader.h"
#include "MyStringBuf.h"

#include <vector>
#include <set>

class FreeTypeFaceWrapper;
class ObjectsContext;

using namespace std;

typedef vector<unsigned int> UIntVector;
typedef set<unsigned int> UIntSet;

class TrueTypeEmbeddedFontWriter
{
public:
	TrueTypeEmbeddedFontWriter(void);
	~TrueTypeEmbeddedFontWriter(void);

	EStatusCode WriteEmbeddedFont(	FreeTypeFaceWrapper& inFontInfo,
									const UIntVector& inSubsetGlyphIDs,
									ObjectIDType inEmbeddedFontObjectID,
									ObjectsContext* inObjectsContext);

private:
	OpenTypeFileInput mTrueTypeInput;
	InputFile mTrueTypeFile;
	OutputStringBufferStream mFontFileStream;
	TrueTypePrimitiveWriter mPrimitivesWriter;
	InputStringBufferStream mFontFileReaderStream; // now this might be confusing - i'm using a reader
											 // to my output buffer, so i can calculate checksums 'n such
	OpenTypePrimitiveReader mPrimitivesReader; // same here...
	unsigned short mSubsetFontGlyphsCount;

	LongFilePositionType mCVTEntryWritingOffset;
	LongFilePositionType mFPGMEntryWritingOffset;
	LongFilePositionType mGLYFEntryWritingOffset;
	LongFilePositionType mHEADEntryWritingOffset;
	LongFilePositionType mHHEAEntryWritingOffset;
	LongFilePositionType mHMTXEntryWritingOffset;
	LongFilePositionType mLOCAEntryWritingOffset;
	LongFilePositionType mMAXPEntryWritingOffset;
	LongFilePositionType mPREPEntryWritingOffset;
	LongFilePositionType mNAMEEntryWritingOffset;
	LongFilePositionType mOS2EntryWritingOffset;
	LongFilePositionType mCMAPEntryWritingOffset;

	LongFilePositionType mHeadCheckSumOffset;
	

	EStatusCode CreateTrueTypeSubset(	FreeTypeFaceWrapper& inFontInfo,
										const UIntVector& inSubsetGlyphIDs,
										MyStringBuf& outFontProgram);

	void AddDependentGlyphs(UIntVector& ioSubsetGlyphIDs);
	bool AddComponentGlyphs(unsigned int inGlyphID,UIntSet& ioComponents);

	EStatusCode WriteTrueTypeHeader();
	unsigned short GetSmallerPower2(unsigned short inNumber);
	unsigned long GetTag(const char* inTagName);
	void WriteEmptyTableEntry(const char* inTag,LongFilePositionType& outEntryPosition);
	EStatusCode WriteHead();
	void WriteTableEntryData(
							LongFilePositionType inTableEntryOffset,
							LongFilePositionType inTableOffset,
							unsigned long inTableLength);
	EStatusCode WriteHHea();
	EStatusCode WriteHMtx();
	EStatusCode WriteMaxp();
	EStatusCode WriteCVT();
	EStatusCode WriteFPGM();
	EStatusCode WritePREP();
	EStatusCode WriteNAME();
	EStatusCode WriteOS2();
	EStatusCode WriteGlyf(const UIntVector& inSubsetGlyphIDs,unsigned long* inLocaTable);
	EStatusCode WriteLoca(unsigned long* inLocaTable);
	EStatusCode WriteCMAP();
	unsigned long GetCheckSum(LongFilePositionType inOffset,unsigned long inLength);
	EStatusCode CreateHeadTableCheckSumAdjustment();
	EStatusCode CreateTableCopy(const char* inTableName,LongFilePositionType inTableEntryLocation);
};
