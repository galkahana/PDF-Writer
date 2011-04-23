#include "StateWriter.h"
#include "ObjectsContext.h"
#include "IByteWriterWithPosition.h"
#include "DictionaryContext.h"
#include "Trace.h"


using namespace IOBasicTypes;

StateWriter::StateWriter(void)
{
	mObjectsContext = NULL;
}

StateWriter::~StateWriter(void)
{
	delete mObjectsContext;
}


EStatusCode StateWriter::Start(const wstring& inStateFilePath)
{

	// open the new file...
	if(mOutputFile.OpenFile(inStateFilePath) != eSuccess)
	{
		TRACE_LOG1("StateWriter::Start, can't open file for state writing in %s",inStateFilePath.c_str());
		return eFailure;
	}
	
	// Get me a new copy of objects context, for this session
	delete mObjectsContext; 
	mObjectsContext = new ObjectsContext();
	mObjectsContext->SetOutputStream(mOutputFile.GetOutputStream());

	// Put up a nice 'lil header comment. nice one, eh?
	mObjectsContext->WriteComment("PDFHummus-1.0");

	// Reset the root object
	mRootObject = 0;

	return eSuccess;
}

ObjectsContext* StateWriter::GetObjectsWriter()
{
	return mObjectsContext;
}

EStatusCode StateWriter::Finish()
{
	EStatusCode status;
	do
	{
		LongFilePositionType xrefTablePosition;

		status = mObjectsContext->WriteXrefTable(xrefTablePosition);
		if(status != 0)
			break;

		WriteTrailerDictionary();
		WriteXrefReference(xrefTablePosition);
		WriteFinalEOF();


	} while(false);
		
	if(eSuccess == status)
		status = mOutputFile.CloseFile();
	else
		mOutputFile.CloseFile();
	return status;	
}

static const string scTrailer = "trailer";
static const string scSize = "Size";
static const string scRoot = "Root";
void StateWriter::WriteTrailerDictionary()
{
	DictionaryContext* dictionaryContext;

	mObjectsContext->WriteKeyword(scTrailer);
	dictionaryContext = mObjectsContext->StartDictionary();
	dictionaryContext->WriteKey(scSize);
	dictionaryContext->WriteIntegerValue(mObjectsContext->GetInDirectObjectsRegistry().GetObjectsCount());
	if(mRootObject !=0)
	{
		dictionaryContext->WriteKey(scRoot);
		dictionaryContext->WriteObjectReferenceValue(mRootObject);
	}
	mObjectsContext->EndDictionary(dictionaryContext);
}

void StateWriter::SetRootObject(ObjectIDType inRootObjectID)
{
	mRootObject = inRootObjectID;
}

static const string scStartXref = "startxref";
void StateWriter::WriteXrefReference(LongFilePositionType inXrefTablePosition)
{
	mObjectsContext->WriteKeyword(scStartXref);
	mObjectsContext->WriteInteger(inXrefTablePosition,eTokenSeparatorEndLine);
}

static const IOBasicTypes::Byte scEOF[] = {'%','%','E','O','F'}; 

void StateWriter::WriteFinalEOF()
{
	IByteWriterWithPosition *freeContextOutput = mObjectsContext->StartFreeContext();
	freeContextOutput->Write(scEOF,5);
	mObjectsContext->EndFreeContext();
}