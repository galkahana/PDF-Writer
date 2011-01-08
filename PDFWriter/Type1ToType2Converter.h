#pragma once

#include "EStatusCode.h"
#include "IType1InterpreterImplementation.h"

#include <map>
#include <set>
#include <vector>

using namespace std;

class IByteWriter;
class Type1Input;

struct ConversionNode
{
	unsigned short mMarkerType;
	LongList mOperands;
};

typedef list<ConversionNode> ConversionNodeList;

struct Stem
{
	Stem(long inOrigin,long inExtent){mOrigin = inOrigin; mExtent = inExtent;}

	long mOrigin;
	long mExtent;
};

class StemLess : public binary_function<const Stem,const Stem,bool>
{
public:
	bool operator( ) (const Stem& inLeft, 
						const Stem& inRight ) const
	{
		if(inLeft.mOrigin == inRight.mOrigin)
			return inLeft.mExtent < inRight.mExtent;
		else
			return inLeft.mOrigin < inRight.mOrigin;
	}
};

typedef set<Stem,StemLess> StemSet;
typedef vector<Stem> StemVector;
typedef set<size_t> SizeTSet;
typedef map<Stem,size_t,StemLess> StemToSizeTMap;

class Type1ToType2Converter : IType1InterpreterImplementation
{
public:
	Type1ToType2Converter(void);
	~Type1ToType2Converter(void);

	EStatusCode WriteConvertedFontProgram(unsigned int inGlyphIndex,
										  Type1Input* inType1Input,
										  IByteWriter* inByteWriter);

	// IType1InterpreterImplementation
	virtual EStatusCode Type1Hstem(const LongList& inOperandList);
	virtual EStatusCode Type1Vstem(const LongList& inOperandList);
	virtual EStatusCode Type1VMoveto(const LongList& inOperandList);
	virtual EStatusCode Type1RLineto(const LongList& inOperandList);
	virtual EStatusCode Type1HLineto(const LongList& inOperandList);
	virtual EStatusCode Type1VLineto(const LongList& inOperandList);
	virtual EStatusCode Type1RRCurveto(const LongList& inOperandList);
	virtual EStatusCode Type1ClosePath(const LongList& inOperandList);
	virtual Type1CharString* GetSubr(long inSubrIndex);
	virtual EStatusCode Type1Return(const LongList& inOperandList);
	virtual EStatusCode Type1Hsbw(const LongList& inOperandList);
	virtual EStatusCode Type1Endchar(const LongList& inOperandList);
	virtual EStatusCode Type1RMoveto(const LongList& inOperandList);
	virtual EStatusCode Type1HMoveto(const LongList& inOperandList);
	virtual EStatusCode Type1VHCurveto(const LongList& inOperandList);
	virtual EStatusCode Type1HVCurveto(const LongList& inOperandList);
	virtual EStatusCode Type1DotSection(const LongList& inOperandList);
	virtual EStatusCode Type1VStem3(const LongList& inOperandList);
	virtual EStatusCode Type1HStem3(const LongList& inOperandList);
	virtual EStatusCode Type1Seac(const LongList& inOperandList);
	virtual EStatusCode Type1Sbw(const LongList& inOperandList);
	virtual EStatusCode Type1Div(const LongList& inOperandList);
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex);
	virtual EStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack);
	virtual EStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack);
	virtual EStatusCode Type1SetCurrentPoint(const LongList& inOperandList);
	virtual EStatusCode Type1InterpretNumber(long inOperand);
	virtual unsigned long GetLenIV();


private:
	Type1Input* mHelper;
	ConversionNodeList mConversionProgram;
	bool mHintReplacementEncountered;
	bool mHintAdditionEncountered;
	bool mFirstPathConstructionEncountered;
	long mSideBearing[2];
	long mWidth[2];

	// hints handling
	StemVector mOrderedHStems;
	StemVector mOrderedVStems;
	StemToSizeTMap mHStems;
	StemToSizeTMap mVStems;
	SizeTSet mCurrentHints;

	// flex handling
	bool mInFlexCollectionMode;
	LongList mFlexParameters;


	EStatusCode RecordOperatorWithParameters(unsigned short inMarkerType,const LongList& inOperandList);
	void RecordOperatorMarker(unsigned short inMarkerType);
	EStatusCode AddHStem(long inOrigin,long inExtent);
	EStatusCode AddVStem(long inOrigin,long inExtent);
	void ConvertStems();
	ConversionNodeList::iterator CollectHintIndexesFromHere(ConversionNodeList::iterator inFirstStemHint);
	ConversionNodeList::iterator InsertOperatorMarker(unsigned short inMarkerType,ConversionNodeList::iterator inInsertPosition);
	void SetupStemHintsInNode(const StemVector& inStems,long inOffsetCoordinate,ConversionNode& refNode);
	bool IsStemHint(unsigned short inMarkerType);
	long GenerateHintMaskFromCollectedHints();
	void ConvertPathConsturction();
	ConversionNodeList::iterator MergeSameOperators(ConversionNodeList::iterator inStartingNode);
	ConversionNodeList::iterator MergeSameOperators(ConversionNodeList::iterator inStartingNode, unsigned short inOpCode);
	ConversionNodeList::iterator MergeAltenratingOperators(ConversionNodeList::iterator inStartingNode,
														   unsigned short inAlternatingOpcode);
	void AddInitialWidthParameter();
	EStatusCode WriteProgramToStream(IByteWriter* inByteWriter);
};
