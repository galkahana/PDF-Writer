#pragma once

#include "EStatusCode.h"
#include "PDFObjectParser.h"
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"

using namespace IOBasicTypes;

class IByteReaderWithPosition;
class PDFDictionary;

#define LINE_BUFFER_SIZE 1024

enum EXrefEntryType
{
	eXrefEntryExisting,
	eXrefEntryDelete,
	eXrefEntryUndefined
};

struct XrefEntryInput
{
	XrefEntryInput(){mObjectPosition = 0;mRivision=0;mType = eXrefEntryUndefined;}

	LongFilePositionType mObjectPosition;
	unsigned long mRivision;
	EXrefEntryType mType;	
};

class PDFParser
{
public:
	PDFParser(void);
	virtual ~PDFParser(void);

	// sets the stream to parse, then parses for enough information to be able
	// to parse objects later
	EStatusCode StartPDFParsing(IByteReaderWithPosition* inSourceStream);

	// get a parser that can parse objects
	PDFObjectParser& GetObjectParser();

	// below become available after initial parsing

	double GetPDFLevel();
	PDFDictionary* GetTrailer();
	// Creates a new object...so you should delete it later
	PDFObject* ParseNewObject(ObjectIDType inObjectId);
	ObjectIDType GetObjectsCount();
	// parses object and checks that it's dictionary
	PDFDictionary* ParseNewDictionaryObject(ObjectIDType inObjectId);



	unsigned long GetPagesCount();
	// don't be confused - number of pages here. returns the dictionary, and verifies that it's actually a page (via type)
	PDFDictionary* ParsePage(unsigned long inPageIndex);

private:
	PDFObjectParser mObjectParser;
	IByteReaderWithPosition* mStream;
	
	// we'll use this items for bacwkards reading. might turns this into a proper stream object
	Byte mLinesBuffer[LINE_BUFFER_SIZE];
	IOBasicTypes::Byte* mCurrentBufferIndex;
	IOBasicTypes::Byte* mLastAvailableIndex;
	LongBufferSizeType mLastReadPositionFromEnd;
	bool mEncounteredFileStart;

	double mPDFLevel;
	LongFilePositionType mLastXrefPosition;
	PDFDictionary* mTrailer;
	ObjectIDType mXrefSize;
	XrefEntryInput* mXrefTable;
	unsigned long mPagesCount;
	ObjectIDType* mPagesObjectIDs;

	EStatusCode ParseHeaderLine();
	EStatusCode ParseEOFLine();
	EStatusCode ParseLastXrefPosition();
	EStatusCode ParseTrailerDictionary();
	EStatusCode BuildXrefTable();
	EStatusCode DetermineXrefSize();
	EStatusCode InitializeXref();
	EStatusCode ParseXref();
	PDFObject* ParseExistingInDirectObject(ObjectIDType inObjectID);
	EStatusCode ParsePagesObjectIDs();
	EStatusCode ParsePagesIDs(PDFDictionary* inPageNode,ObjectIDType inNodeObjectID);
	EStatusCode ParsePagesIDs(PDFDictionary* inPageNode,ObjectIDType inNodeObjectID,unsigned long& ioCurrentPageIndex);

	void ResetParser();


	// Backward reading
	bool ReadNextBufferFromEnd();
	LongBufferSizeType GetCurrentPositionFromEnd();
	bool ReadBack(Byte& outValue);
	bool IsBeginOfFile();

	bool GoBackTillToken();
	void GoBackTillLineStart();
	bool IsPDFWhiteSpace(Byte inCharacter);

};
