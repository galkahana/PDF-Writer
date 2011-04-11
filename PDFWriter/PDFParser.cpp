#include "PDFParser.h"
#include "IByteReaderWithPosition.h"
#include "PDFParserTokenizer.h"
#include "Trace.h"
#include "PDFInteger.h"
#include "PDFObject.h"
#include "PDFSymbol.h"
#include "BoxingBase.h"
#include "PDFDictionary.h"
#include "BoxingBase.h"
#include "PDFIndirectObjectReference.h"
#include "PDFName.h"
#include "PDFArray.h"
#include "RefCountPtr.h"
#include "PDFObjectCast.h"

PDFParser::PDFParser(void)
{
	mStream = NULL;
	mTrailer = NULL;
	mXrefTable = NULL;
	mPagesObjectIDs = NULL;
}

PDFParser::~PDFParser(void)
{
	ResetParser();
}

void PDFParser::ResetParser()
{
	mTrailer = NULL;
	delete[] mXrefTable;
	mXrefTable = NULL;
	delete[] mPagesObjectIDs;

}

EStatusCode PDFParser::StartPDFParsing(IByteReaderWithPosition* inSourceStream)
{
	EStatusCode status;


	ResetParser();

	mObjectParser.SetReadStream(inSourceStream);
	mStream = inSourceStream;

	do
	{
		status = ParseHeaderLine();
		if(status != eSuccess)
			break;

		// initialize reading from end
		mLastReadPositionFromEnd = 0;
		mEncounteredFileStart = false;
		mLastAvailableIndex = mCurrentBufferIndex = mLinesBuffer;

		status = ParseEOFLine();
		if(status != eSuccess)
			break;

		status = ParseLastXrefPosition();
		if(status != eSuccess)
			break;

		status = ParseTrailerDictionary();
		if(status != eSuccess)
			break;

		status = BuildXrefTable();
		if(status != eSuccess)
			break;

		status = ParsePagesObjectIDs();
		if(status != eSuccess)
			break;

	}while(false);

	return status;
}

PDFObjectParser& PDFParser::GetObjectParser()
{
	return mObjectParser;
}

static const string scPDFMagic = "%PDF-";
EStatusCode PDFParser::ParseHeaderLine()
{
	PDFParserTokenizer tokenizer;

	tokenizer.SetReadStream(mStream);
	BoolAndString tokenizerResult = tokenizer.GetNextToken();

	if(!tokenizerResult.first)
	{
		TRACE_LOG("PDFParser::ParseHeaderLine, no tokens in PDF input. in other words - it's empty.");
		return eFailure;
	}

	if(tokenizerResult.second.compare(0,scPDFMagic.size(),scPDFMagic) != 0)
	{
		TRACE_LOG1("PDFParser::ParseHeaderLine, file does not begin as a PDF file. a PDF file should start with \"%PDF-\". file header = %s",tokenizerResult.second);
		return eFailure;
	}

	mPDFLevel = Double(tokenizerResult.second.substr(scPDFMagic.size()));
	return eSuccess;
}

static const string scEOF = "%%EOF";
EStatusCode PDFParser::ParseEOFLine()
{
	/* go back till you hit token. this should be the EOF. go back till line start and get the token...if it's not EOF, fail.
	   since EOF is a comment, then if there's anything else in that line it will either be before %%EOF, which means %%EOF won't be taken, or after - 
	   in which case it'd be part of the comment. in any case - if it's not exactly EOF, there will be a failure. but i am allowing 
	   extra empty lines after %%EOF
	*/
	if(GoBackTillToken())
	{
		GoBackTillLineStart();
		mStream->SetPositionFromEnd(GetCurrentPositionFromEnd());

		PDFParserTokenizer aTokenizer;
		aTokenizer.SetReadStream(mStream);
		BoolAndString token = aTokenizer.GetNextToken();

		if(token.first && (scEOF == token.second))
		{
			return eSuccess;
		}
		else
		{
			TRACE_LOG("PDFParser::ParseEOFLine, failure, last line not %%EOF");
			return eFailure;
		}
	}
	else
	{
		TRACE_LOG("PDFParser::ParseEOFLine, Couldn't find tokens in file");
		return eFailure;
	}
}

LongBufferSizeType PDFParser::GetCurrentPositionFromEnd()
{
	return mLastReadPositionFromEnd-(mCurrentBufferIndex-mLinesBuffer);
}

bool PDFParser::GoBackTillToken()
{
	Byte buffer;
	bool foundToken = false;

	while(ReadBack(buffer))
	{
		if(!IsPDFWhiteSpace(buffer))
		{
			foundToken = true;
			break;
		}
	}
	return foundToken;
}

static const Byte scWhiteSpaces[] = {0,0x9,0xA,0xC,0xD,0x20};
bool PDFParser::IsPDFWhiteSpace(Byte inCharacter)
{
	bool isWhiteSpace = false;
	for(int i=0; i < 6 && !isWhiteSpace; ++i)
		isWhiteSpace =  (scWhiteSpaces[i] == inCharacter);
	return isWhiteSpace;
}


static const char scCR = '\r';
static const char scLN = '\n';
void PDFParser::GoBackTillLineStart()
{
	Byte buffer;

	while(ReadBack(buffer))
	{
		if(scLN == buffer || scCR == buffer)
			break;
	}
}

bool PDFParser::ReadBack(Byte& outValue)
{
	if(IsBeginOfFile())
		return false;

	if(mCurrentBufferIndex > mLinesBuffer)
	{
		--mCurrentBufferIndex;
		outValue = *mCurrentBufferIndex;
		return true;
	}
	else
	{
		ReadNextBufferFromEnd(); // must be able to read...but could be 0 bytes
		if(mCurrentBufferIndex > mLinesBuffer)
		{
			--mCurrentBufferIndex;
			outValue = *mCurrentBufferIndex;
			return true;
		}
		else
			return false;
	}
}

bool PDFParser::ReadNextBufferFromEnd()
{
	if(mEncounteredFileStart)
	{
		return false;
	}
	else
	{
		mStream->SetPositionFromEnd(mLastReadPositionFromEnd + LINE_BUFFER_SIZE);
		LongBufferSizeType readAmount = mStream->Read(mLinesBuffer,LINE_BUFFER_SIZE);
		if(0 == readAmount)
			return false;
		mLastAvailableIndex = mLinesBuffer + readAmount;
		mCurrentBufferIndex = mLastAvailableIndex;
		mLastReadPositionFromEnd+= readAmount;
		mEncounteredFileStart = readAmount < LINE_BUFFER_SIZE;
		return true;
	}
}

bool PDFParser::IsBeginOfFile()
{
	return mEncounteredFileStart && (mCurrentBufferIndex == mLinesBuffer);
}

static const string scStartxref = "startxref";
EStatusCode PDFParser::ParseLastXrefPosition()
{
	EStatusCode status = eSuccess;
	
	// next two lines should be the xref position and then "startxref"
	
	do
	{

		// find and read xref position
		if(!GoBackTillToken())
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseXrefPosition, couldn't find xref position token");
			break;
		}

		GoBackTillLineStart();
		mStream->SetPositionFromEnd(GetCurrentPositionFromEnd());
		
		mObjectParser.ResetReadState();
		PDFObjectCastPtr<PDFInteger> xrefPosition(mObjectParser.ParseNewObject());
		if(!xrefPosition)
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseXrefPosition, syntax error in reading xref position");
			break;
		}

		mLastXrefPosition = (LongFilePositionType)xrefPosition->GetValue();

		// find and read startxref keyword
		if(!GoBackTillToken())
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseXrefPosition, couldn't find startxref keyword");
			break;
		}

		GoBackTillLineStart();
		mStream->SetPositionFromEnd(GetCurrentPositionFromEnd());
		
		mObjectParser.ResetReadState();
		PDFObjectCastPtr<PDFSymbol> startxRef(mObjectParser.ParseNewObject());

		if(!startxRef || startxRef->GetValue() != scStartxref)
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseXrefPosition, syntax error in reading xref position");
			break;
		}

	}while(false);

	return status;

}

static const string scDictionaryClose = ">>";
static const string scDictionaryStart = "<<";
static const string scTrailer = "trailer";
EStatusCode PDFParser::ParseTrailerDictionary()
{
	// K. here's a little tricky part. so i'm gonna be more strict
	// i'm going up one line, and the line must be ">>"
	// then going up lines till i hit a line which is equal to "<<"
	// then up one line and it must be the keyword "trailer"
	// then ParseNewObject should give us the trailer dictionary.

	EStatusCode status = eSuccess;
	PDFParserTokenizer aTokenizer;
	aTokenizer.SetReadStream(mStream);

	do
	{
		if(!GoBackTillToken())
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseTrailerDictionary, couldn't find trailer end token");
			break;
		}

		GoBackTillLineStart();
		mStream->SetPositionFromEnd(GetCurrentPositionFromEnd());

		BoolAndString token = aTokenizer.GetNextToken();
		if(!token.first || token.second != scDictionaryClose)
		{
			status = eFailure;
			TRACE_LOG1("PDFParser::ParseTrailerDictionary, unexpected token in find trailer end. token = %s",token.second.c_str());
			break;
		}

		// now loop till you get to dictionar start 
		//(note that no nesting is expected due to specifications where all must be indirect, so no need to consider nesting.)
		bool foundDictionaryStart = false;
		while(!foundDictionaryStart)
		{
			if(!GoBackTillToken())
			{
				status = eFailure;
				TRACE_LOG("PDFParser::ParseTrailerDictionary, couldn't find token while looking for trailer dictionary start");
				break;
			}
			GoBackTillLineStart();
			mStream->SetPositionFromEnd(GetCurrentPositionFromEnd());

			aTokenizer.ResetReadState();
			BoolAndString token = aTokenizer.GetNextToken();
			if(!token.first)
			{
				status = eFailure;
				TRACE_LOG("PDFParser::ParseTrailerDictionary, unexpected token read error");
				break;
			}
			
			foundDictionaryStart = (token.second == scDictionaryStart);
		}

		if(!foundDictionaryStart)
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseTrailerDictionary, couldn't find trailer dictionary start");
			break;
		}

		// k. next thing is to just make sure that the previous token is trailer, and we are set
		if(!GoBackTillToken())
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseTrailerDictionary, couldn't find trailer keyword");
			break;
		}

		GoBackTillLineStart();
		mStream->SetPositionFromEnd(GetCurrentPositionFromEnd());

		aTokenizer.ResetReadState();
		token = aTokenizer.GetNextToken();
		if(!token.first || token.second != scTrailer)
		{
			status = eFailure;
			TRACE_LOG1("PDFParser::ParseTrailerDictionary, unexpected token instead of trailer keword. token = %s",token.second.c_str());
			break;
		}

		// k. now that all is well, just parse the damn dictionary, which is actually...the easiest part.
		PDFObjectCastPtr<PDFDictionary> dictionaryObject(mObjectParser.ParseNewObject());
		if(!dictionaryObject)
		{
			status = eFailure;
			TRACE_LOG("PDFParser::ParseTrailerDictionary, failure to parse trailer dictionary");
			break;
		}

		mTrailer = dictionaryObject;
	}while(false);

	return status;
}

EStatusCode PDFParser::BuildXrefTable()
{
	EStatusCode status;

	do
	{
		status = DetermineXrefSize();
		if(status != eSuccess)
			break;

		status = InitializeXref();
		if(status != eSuccess)
			break;

		status = ParseXref();
		if(status != eSuccess)
			break;

	}while(false);

	return status;
}

EStatusCode PDFParser::DetermineXrefSize()
{
	PDFObjectCastPtr<PDFInteger> aSize(mTrailer->QueryDirectObject("Size"));

	if(!aSize)
	{
		return eFailure;
	}
	else
	{
		mXrefSize = (ObjectIDType)aSize->GetValue();
		return eSuccess;
	}
}

EStatusCode PDFParser::InitializeXref()
{
	mXrefTable = new XrefEntryInput[mXrefSize];
	return eSuccess;
}

typedef BoxingBaseWithRW<ObjectIDType> ObjectIDTypeBox;
typedef BoxingBaseWithRW<unsigned long> ULong;
typedef BoxingBaseWithRW<LongFilePositionType> LongFilePositionTypeBox;

static const string scXref = "xref";
EStatusCode PDFParser::ParseXref()
{
	// K. cross ref starts at  xref position
	// and ends with trailer (or when exahausted the number of objects...whichever first)
	// i'm gonna tokanize them, for easier reading
	PDFParserTokenizer tokenizer;
	BoolAndString token;
	EStatusCode status = eSuccess;
	ObjectIDType firstNonSectionObject;
	Byte entry[20];

	tokenizer.SetReadStream(mStream);
	mStream->SetPosition(mLastXrefPosition);

	// i'll just use a tokenizer, to save perofrmance, and assume that the file is correct

	do
	{
		// first token must be "xref", so just verify
		token = tokenizer.GetNextToken();
		if(!token.first || token.second != scXref)
		{
			TRACE_LOG1("PDFParser::ParseXref, error in parsing xref, expected to find \"xref\" keyword, found = %s",token.second.c_str());
			status = eFailure;
			break;
		}
		
		ObjectIDType currentObject = 0;

		while(currentObject < mXrefSize && eSuccess == status)
		{
			token = tokenizer.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("PDFParser::ParseXref, failed to read tokens, while reading xref");
				status = eFailure;
				break;
			}
			
			// token may be either start of section or "trailer"
			if(scTrailer == token.second)
				break;

			currentObject = ObjectIDTypeBox(token.second);
			token = tokenizer.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("PDFParser::ParseXref, unable to read section size, while reading xref");
				status = eFailure;
				break;
			}
			firstNonSectionObject = currentObject + ObjectIDTypeBox(token.second);

			// now parse the section. 
			while(currentObject < firstNonSectionObject)
			{
				if(mStream->Read(entry,20) != 20)
				{
					TRACE_LOG("PDFParser::ParseXref, failed to read xref entry");
					status = eFailure;
					break;
				}
				mXrefTable[currentObject].mObjectPosition = LongFilePositionTypeBox((const char*)entry);
				mXrefTable[currentObject].mRivision = ULong((const char*)(entry+11));
				mXrefTable[currentObject].mType = entry[18] == 'n' ? eXrefEntryExisting:eXrefEntryDelete;
				++currentObject;
			}
		}
		if(status != eSuccess)
			break;


	}while(false);	
	return status;
}

PDFDictionary* PDFParser::GetTrailer()
{
	return mTrailer.GetPtr();
}

double PDFParser::GetPDFLevel()
{
	return mPDFLevel;
}

PDFObject* PDFParser::ParseNewObject(ObjectIDType inObjectId)
{
	if(inObjectId >= mXrefSize)
	{
		return NULL;
	}
	else if(eXrefEntryExisting == mXrefTable[inObjectId].mType)
	{
		return ParseExistingInDirectObject(inObjectId);
	}
	else
		return NULL;
}

ObjectIDType PDFParser::GetObjectsCount()
{
	return mXrefSize;
}

static const string scObj = "obj";
PDFObject* PDFParser::ParseExistingInDirectObject(ObjectIDType inObjectID)
{
	PDFObject* readObject = NULL;

	mStream->SetPosition(mXrefTable[inObjectID].mObjectPosition);
	mObjectParser.ResetReadState();


	do
	{
		// should get us to the ObjectNumber ObjectVersion obj section
		// verify that it's good and if so continue to parse the object itself

		// verify object ID
		PDFObjectCastPtr<PDFInteger> idObject(mObjectParser.ParseNewObject());

		if(!idObject)
		{
			TRACE_LOG("PDFParser::ParseExistingInDirectObject, failed to read object declaration, ID");
			break;
		}

		if(idObject->GetValue() != inObjectID)
		{
			TRACE_LOG2("PDFParser::ParseExistingInDirectObject, failed to read object declaration, exepected ID = %ld, found %ld",
				inObjectID,idObject->GetValue());
			break;
		}

		// verify object Version
		PDFObjectCastPtr<PDFInteger> versionObject(mObjectParser.ParseNewObject());

		if(!versionObject)
		{
			TRACE_LOG("PDFParser::ParseExistingInDirectObject, failed to read object declaration, Version");
			break;
		}

		if(versionObject->GetValue() != mXrefTable[inObjectID].mRivision)
		{
			TRACE_LOG2("PDFParser::ParseExistingInDirectObject, failed to read object declaration, exepected version = %ld, found %ld",
				mXrefTable[inObjectID].mRivision,versionObject->GetValue());
			break;
		}

		// now the obj keyword
		PDFObjectCastPtr<PDFSymbol> objKeyword(mObjectParser.ParseNewObject());

		if(!objKeyword)
		{
			TRACE_LOG("PDFParser::ParseExistingInDirectObject, failed to read object declaration, obj keyword");
			break;
		}

		if(objKeyword->GetValue() != scObj)
		{
			TRACE_LOG1("PDFParser::ParseExistingInDirectObject, failed to read object declaration, expected obj keyword found %s",
				objKeyword->GetValue().c_str());
			break;
		}

		readObject = mObjectParser.ParseNewObject();

	}while(false);

	return readObject;
}

EStatusCode PDFParser::ParsePagesObjectIDs()
{
	EStatusCode status = eSuccess;

	// m.k plan is to look for the catalog, then find the pages, then initialize the array to the count at the root, and then just recursively loop
	// the pages by order of pages and fill up the IDs. easy.
	
	do
	{
		// get catalogue, verify indirect reference
		PDFObjectCastPtr<PDFIndirectObjectReference> catalogReference(mTrailer->QueryDirectObject("Root"));
		if(!catalogReference)
		{
			TRACE_LOG("PDFParser::ParsePagesObjectIDs, failed to read catalog reference in trailer");
			status = eFailure;
			break;
		}

		PDFObjectCastPtr<PDFDictionary> catalog(ParseNewObject(catalogReference->mObjectID));
		if(!catalog)
		{
			TRACE_LOG("PDFParser::ParsePagesObjectIDs, failed to read catalog");
			status = eFailure;
			break;
		}

		// get pages, verify indirect reference
		PDFObjectCastPtr<PDFIndirectObjectReference> pagesReference(catalog->QueryDirectObject("Pages"));
		if(!pagesReference)
		{
			TRACE_LOG("PDFParser::ParsePagesObjectIDs, failed to read pages reference in catalog");
			status = eFailure;
			break;
		}

		PDFObjectCastPtr<PDFDictionary> pages(ParseNewObject(pagesReference->mObjectID));
		if(!pages)
		{
			TRACE_LOG("PDFParser::ParsePagesObjectIDs, failed to read pages");
			status = eFailure;
			break;
		}

		PDFObjectCastPtr<PDFInteger> totalPagesCount(QueryDictionaryObject(pages.GetPtr(),"Count"));
		if(!totalPagesCount)
		{
			TRACE_LOG("PDFParser::ParsePagesObjectIDs, failed to read pages count");
			status = eFailure;
			break;
		}
	
		mPagesCount = totalPagesCount->GetValue();
		mPagesObjectIDs = new ObjectIDType[mPagesCount];

		// now iterate through pages objects, and fill up the IDs [don't really need the object ID for the root pages tree...but whatever
		status = ParsePagesIDs(pages.GetPtr(),pagesReference->mObjectID);

	}while(false);

	return status;
}

EStatusCode PDFParser::ParsePagesIDs(PDFDictionary* inPageNode,ObjectIDType inNodeObjectID)
{
	unsigned long currentPageIndex = 0;

	return ParsePagesIDs(inPageNode,inNodeObjectID,currentPageIndex);
}

static const string scPage = "Page";
static const string scPages = "Pages";
EStatusCode PDFParser::ParsePagesIDs(PDFDictionary* inPageNode,ObjectIDType inNodeObjectID,unsigned long& ioCurrentPageIndex)
{
	// recursion.
	// if this is a page, write it's node object ID in the current page index and +1
	// if this is a pagetree, loop it's kids, for each parsing the kid, running the recursion on it, and deleting

	EStatusCode status = eSuccess;

	do
	{
		PDFObjectCastPtr<PDFName> objectType(inPageNode->QueryDirectObject("Type"));
		if(!objectType)
		{
			TRACE_LOG("PDFParser::ParsePagesIDs, can't read object type");
			status = eFailure;
			break;
		}
		
		if(scPage == objectType->GetValue())
		{
			// a Page
			if(ioCurrentPageIndex >= mPagesCount)
			{
				TRACE_LOG("PDFParser::ParsePagesIDs, there are more pages than the page count specifies. fail.");
				status = eFailure;
				break;
			}

			mPagesObjectIDs[ioCurrentPageIndex] = inNodeObjectID;
			++ioCurrentPageIndex;
		}
		else if(scPages == objectType->GetValue())
		{
			// a Page tree node
			PDFObjectCastPtr<PDFArray> kidsObject(inPageNode->QueryDirectObject("Kids"));
			if(!kidsObject)
			{
				TRACE_LOG("PDFParser::ParsePagesIDs, unable to find page kids array");
				status = eFailure;
				break;
			}

			SingleValueContainerIterator<PDFObjectVector> it = kidsObject->GetIterator();
			
			while(it.MoveNext() && eSuccess == status)
			{
				if(it.GetItem()->GetType() != ePDFObjectIndirectObjectReference)
				{
					TRACE_LOG1("PDFParser::ParsePagesIDs, unexpected type for a Kids array object, type = %s",scPDFObjectTypeLabel[it.GetItem()->GetType()]);
					status = eFailure;
					break;
				}

				PDFObjectCastPtr<PDFDictionary> pageNodeObject(ParseNewObject(((PDFIndirectObjectReference*)it.GetItem())->mObjectID));
				if(!pageNodeObject)
				{
					TRACE_LOG("PDFParser::ParsePagesIDs, unable to parse page node object from kids reference");
					status = eFailure;
					break;
				}

				status = ParsePagesIDs(pageNodeObject.GetPtr(),((PDFIndirectObjectReference*)it.GetItem())->mObjectID,ioCurrentPageIndex);
			}
		}
		else 
		{
			TRACE_LOG1("PDFParser::ParsePagesIDs, unexpected object type. should be either Page or Pages, found %s",objectType->GetValue().c_str());
			status = eFailure;
			break;
		}
	}while(false);

	return status;
}

unsigned long  PDFParser::GetPagesCount()
{
	return mPagesCount;
}

PDFDictionary* PDFParser::ParsePage(unsigned long inPageIndex)
{
	if(mPagesCount <= inPageIndex)
		return NULL;

	PDFObjectCastPtr<PDFDictionary> pageObject(ParseNewObject(mPagesObjectIDs[inPageIndex]));

	if(!pageObject)
	{
		TRACE_LOG1("PDFParser::ParsePage, couldn't find page object for index %ld",inPageIndex);
		return NULL;
	}

	PDFObjectCastPtr<PDFName> objectType(pageObject->QueryDirectObject("Type"));

	if(scPage == objectType->GetValue())
	{
		pageObject->AddRef();
		return pageObject.GetPtr();
	}
	else
	{
		TRACE_LOG1("PDFParser::ParsePage, page object listed in page array for %ld is actually not a page",inPageIndex);
		return NULL;
	}
}

PDFObject* PDFParser::QueryDictionaryObject(PDFDictionary* inDictionary,const string& inName)
{
	RefCountPtr<PDFObject> anObject(inDictionary->QueryDirectObject(inName));

	if(anObject.GetPtr() == NULL)
		return NULL;

	if(anObject->GetType() == ePDFObjectIndirectObjectReference)
	{
		PDFObject* theActualObject = ParseNewObject(((PDFIndirectObjectReference*)anObject.GetPtr())->mObjectID);
		return theActualObject;
	}
	else
	{
		anObject->AddRef(); // adding ref to increase owners
		return anObject.GetPtr();
	}
}

PDFObject* PDFParser::QueryArrayObject(PDFArray* inArray,unsigned long inIndex)
{
	RefCountPtr<PDFObject> anObject(inArray->QueryObject(inIndex));

	if(anObject.GetPtr() == NULL)
		return NULL;
	
	if(anObject->GetType() == ePDFObjectIndirectObjectReference)
	{
		PDFObject* theActualObject = ParseNewObject(((PDFIndirectObjectReference*)anObject.GetPtr())->mObjectID);
		return theActualObject;
	}
	else
	{
		anObject->AddRef(); // adding ref to increase owners
		return anObject.GetPtr();
	}

}

