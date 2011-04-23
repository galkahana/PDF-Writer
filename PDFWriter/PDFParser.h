#pragma once

#include "EStatusCode.h"
#include "PDFObjectParser.h"
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"
#include "RefCountPtr.h"
#include "PDFDictionary.h"
#include "IByteReaderWithPosition.h"
#include "AdapterIByteReaderWithPositionToIReadPositionProvider.h"

#include <map>

using namespace IOBasicTypes;

class PDFArray;
class PDFStreamInput;

#define LINE_BUFFER_SIZE 1024

enum EXrefEntryType
{
	eXrefEntryExisting,
	eXrefEntryDelete,
	eXrefEntryStreamObject,
	eXrefEntryUndefined
};

struct XrefEntryInput
{
	XrefEntryInput(){mObjectPosition = 0;mRivision=0;mType = eXrefEntryUndefined;}

	// well...it's more like...the first number in a pair on an xref, and the second one. the names
	// are true only for "n" type of entries
	LongFilePositionType mObjectPosition;
	unsigned long mRivision;
	EXrefEntryType mType;	
};

struct ObjectStreamHeaderEntry
{
	ObjectIDType mObjectNumber;
	LongFilePositionType mObjectOffset;
};

class ReadPositionProviderForStreamWithPosition : public IReadPositionProvider
{
public:
	void Assign(IByteReaderWithPosition* inStream)
	{
		mStream = inStream;
	}

	virtual LongFilePositionType GetCurrentPosition()
	{
		return mStream->GetCurrentPosition();
	}
private:
	IByteReaderWithPosition* mStream;
	
};

typedef map<ObjectIDType,ObjectStreamHeaderEntry*> ObjectIDTypeToObjectStreamHeaderEntryMap;

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

	// below become available after initial parsing [this level is from the header]
	double GetPDFLevel();

	// GetTrailer, not calling AddRef
	PDFDictionary* GetTrailer();

	// IMPORTANT! All non "Get" prefix methods below return an object after calling AddRef (or at least make sure reference is added)
	// to handle refcount use the RefCountPtr object, or just make sure to call Release when you are done.
	
	// Creates a new object, use smart pointers to control ownership
	PDFObject* ParseNewObject(ObjectIDType inObjectId);
	ObjectIDType GetObjectsCount();

	// Query a dictinary object, if indirect, go and fetch the indirect object and return it instead
	// [if you want the direct dictionary value, use PDFDictionary::QueryDirectObject [will AddRef automatically]
	PDFObject* QueryDictionaryObject(PDFDictionary* inDictionary,const string& inName);
	
	// Query an array object, if indirect, go and fetch the indirect object and return it instead
	// [if you want the direct array value, use the PDFArray direct access to the vector [and use AddRef, cause it won't]
	PDFObject* QueryArrayObject(PDFArray* inArray,unsigned long inIndex);

	unsigned long GetPagesCount();
	// don't be confused - pass number of pages here. returns the dictionary, and verifies that it's actually a page (via type)
	PDFDictionary* ParsePage(unsigned long inPageIndex);

	// This is sort of a public function, in order to provide the right filters when reading
	// note that the final reader does not own the source stream. You can delete it safely after usage
	// even if it's cascading filters
	IByteReader* CreateInputStreamReader(PDFStreamInput* inStream);

	// use this to explictly free used objects. quite obviously this means that you'll have to parse the file again
	void ResetParser();

	// using PDFParser also for state information reading. this is a specialized version of the StartParsing for reading state
	EStatusCode StartStateFileParsing(IByteReaderWithPosition* inSourceStream);
private:
	PDFObjectParser mObjectParser;
	IByteReaderWithPosition* mStream;
	AdapterIByteReaderWithPositionToIReadPositionProvider mCurrentPositionProvider;
	
	// we'll use this items for bacwkards reading. might turns this into a proper stream object
	Byte mLinesBuffer[LINE_BUFFER_SIZE];
	IOBasicTypes::Byte* mCurrentBufferIndex;
	IOBasicTypes::Byte* mLastAvailableIndex;
	LongBufferSizeType mLastReadPositionFromEnd;
	bool mEncounteredFileStart;
	ObjectIDTypeToObjectStreamHeaderEntryMap mObjectStreamsCache;

	double mPDFLevel;
	LongFilePositionType mLastXrefPosition;
	RefCountPtr<PDFDictionary> mTrailer;
	ObjectIDType mXrefSize;
	XrefEntryInput* mXrefTable;
	unsigned long mPagesCount;
	ObjectIDType* mPagesObjectIDs;

	EStatusCode ParseHeaderLine();
	EStatusCode ParseEOFLine();
	EStatusCode ParseLastXrefPosition();
	EStatusCode ParseTrailerDictionary();
	EStatusCode BuildXrefTableFromTable();
	EStatusCode DetermineXrefSize();
	EStatusCode InitializeXref();
	EStatusCode ParseXrefFromXrefTable(XrefEntryInput* inXrefTable,ObjectIDType inXrefSize,LongFilePositionType inXrefPosition);
	PDFObject*  ParseExistingInDirectObject(ObjectIDType inObjectID);
	EStatusCode ParsePagesObjectIDs();
	EStatusCode ParsePagesIDs(PDFDictionary* inPageNode,ObjectIDType inNodeObjectID);
	EStatusCode ParsePagesIDs(PDFDictionary* inPageNode,ObjectIDType inNodeObjectID,unsigned long& ioCurrentPageIndex);
	EStatusCode ParsePreviousXrefs(PDFDictionary* inTrailer);
	void MergeXrefWithMainXref(XrefEntryInput* inTableToMerge);
	EStatusCode ParseFileDirectory();
	EStatusCode BuildXrefTableAndTrailerFromXrefStream();
	// an overload for cases where the xref stream object is already parsed
	EStatusCode ParseXrefFromXrefStream(XrefEntryInput* inXrefTable,ObjectIDType inXrefSize,PDFStreamInput* inXrefStream);
	// an overload for cases where the position should hold a stream object, and it should be parsed
	EStatusCode ParseXrefFromXrefStream(XrefEntryInput* inXrefTable,ObjectIDType inXrefSize,LongFilePositionType inXrefPosition);
	EStatusCode ReadXrefStreamSegment(XrefEntryInput* inXrefTable,
									 ObjectIDType inSegmentStartObject,
									 ObjectIDType inSegmentCount,
									 IByteReader* inReadFrom,
									 int* inEntryWidths,
									 unsigned long inEntryWidthsSize);
	EStatusCode ReadXrefSegmentValue(IByteReader* inSource,int inEntrySize,long long& outValue);
	EStatusCode ReadXrefSegmentValue(IByteReader* inSource,int inEntrySize,ObjectIDType& outValue);
	EStatusCode ParseDirectory(LongFilePositionType inXrefPosition,XrefEntryInput* inXrefTable,ObjectIDType inXrefSize,PDFDictionary** outTrailer);
	PDFObject* ParseExistingInDirectStreamObject(ObjectIDType inObjectId);
	EStatusCode ParseObjectStreamHeader(ObjectStreamHeaderEntry* inHeaderInfo,ObjectIDType inObjectsCount);
	void MovePositionInStream(LongFilePositionType inPosition);

	// Backward reading
	bool ReadNextBufferFromEnd();
	LongBufferSizeType GetCurrentPositionFromEnd();
	bool ReadBack(Byte& outValue);
	bool IsBeginOfFile();

	bool GoBackTillToken();
	bool GoBackTillNonToken();
	void GoBackTillLineStart();
	bool IsPDFWhiteSpace(Byte inCharacter);

};
