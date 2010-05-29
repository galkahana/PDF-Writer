#include "ObjectsContext.h"
#include "IOBasicTypes.h"
#include "IByteWriterWithPosition.h"
#include "SafeBufferMacrosDefs.h"
#include "DictionaryContext.h"
#include "Trace.h"
#include "OutputStreamTraits.h"
#include "PDFStream.h"

ObjectsContext::ObjectsContext(void)
{
	mOutputStream = NULL;
}

ObjectsContext::~ObjectsContext(void)
{
}


void ObjectsContext::SetOutputStream(IByteWriterWithPosition* inOutputStream)
{
	mOutputStream = inOutputStream;
	mPrimitiveWriter.SetStreamForWriting(inOutputStream);
}

static const IOBasicTypes::Byte scComment[1] = {'%'};
void ObjectsContext::WriteComment(const string& inCommentText)
{
	mOutputStream->Write(scComment,1);
	mOutputStream->Write((const IOBasicTypes::Byte *)inCommentText.c_str(),inCommentText.size());
	EndLine();
}

void ObjectsContext::WriteName(const string& inName,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteName(inName,inSeparate);
}

void ObjectsContext::WriteLiteralString(const string& inString,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteLiteralString(inString,inSeparate);
}

static const IOBasicTypes::Byte scLeftAngle[1] = {'<'};
static const IOBasicTypes::Byte scRightAngle[1] = {'>'};

void ObjectsContext::WriteHexString(const string& inString,ETokenSeparator inSeparate)
{
	mOutputStream->Write(scLeftAngle,1);
	mOutputStream->Write((const IOBasicTypes::Byte *)inString.c_str(),inString.size());
	mOutputStream->Write(scRightAngle,1);
	WriteTokenSeparator(inSeparate);
}

static const IOBasicTypes::Byte scR[1] = {'R'};
void ObjectsContext::WriteIndirectObjectReference(ObjectIDType inIndirectObjectID,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteInteger(inIndirectObjectID);
	mPrimitiveWriter.WriteInteger(0);
	mOutputStream->Write(scR,1);
	mPrimitiveWriter.WriteTokenSeparator(inSeparate);
}

IByteWriterWithPosition* ObjectsContext::StartFreeContext()
{
	return mOutputStream;
}

void ObjectsContext::EndFreeContext()
{
	// currently just a marker, do nothing. allegedly used to return to a "controlled" context
}

static const IOBasicTypes::Byte scXref[] = {'x','r','e','f'};
static const IOBasicTypes::Byte scFreeObjectEntry[] = {'0','0','0','0','0','0','0','0','0','0',' ','6','5','5','3','5',' ','f','\r','\n'};

EStatusCode ObjectsContext::WriteXrefTable(LongFilePositionType& outWritePosition)
{
	EStatusCode status = eSuccess;
	outWritePosition = mOutputStream->GetCurrentPosition();
	
	// write xref keyword
	mOutputStream->Write(scXref,4);
	mPrimitiveWriter.EndLine();

	// write objects amount in subsection (just one subsection required)
	mPrimitiveWriter.WriteInteger(0);
	mPrimitiveWriter.WriteInteger(mReferencesRegistry.GetObjectsCount(),eTokenSeparatorEndLine);

	// write single free object entry
	mOutputStream->Write(scFreeObjectEntry,20);

	// write used objects
	char entryBuffer[21];

	for(ObjectIDType i = 1; i < mReferencesRegistry.GetObjectsCount() && (eSuccess == status);++i)
	{
		const ObjectWriteInformation& objectReference = mReferencesRegistry.GetNthObjectReference(i);
		if(objectReference.mObjectWritten)
		{
			SAFE_SPRINTF_1(entryBuffer,21,"%010lld 00000 n\r\n",objectReference.mWritePosition);
			mOutputStream->Write((const IOBasicTypes::Byte *)entryBuffer,20);
		}
		else
		{
			// object not written. at this point this should not happen, and indicates a failure
			status = eFailure;
			TRACE_LOG1("ObjectsContext::WriteXrefTable, Unexpected Failure. Object of ID = %ld was not registered as written. probably means it was not written",i);
		}
	}
	return status;
}

DictionaryContext* ObjectsContext::StartDictionary()
{
	DictionaryContext* newDictionary = new DictionaryContext(this,mDictionaryStack.size());

	mDictionaryStack.push_back(newDictionary);
	return newDictionary;
}

EStatusCode ObjectsContext::EndDictionary(DictionaryContext* ObjectsContext)
{
	if(mDictionaryStack.size() > 0)
	{
		if(mDictionaryStack.back() == ObjectsContext)
		{
			delete mDictionaryStack.back();
			mDictionaryStack.pop_back();
			return eSuccess;
		}
		else
		{
			TRACE_LOG("ObjectsContext::EndDictionary, nesting violation. Trying to close a dictionary while one of it's children is still open. First End the children");
			return eFailure;
		}
	}
	else
	{
		TRACE_LOG("ObjectsContext::EndDictionary, stack underflow. Trying to end a dictionary when there's no open dictionaries");
		return eFailure;
	}
}

IndirectObjectsReferenceRegistry& ObjectsContext::GetInDirectObjectsRegistry()
{
	return mReferencesRegistry;
}


void ObjectsContext::EndLine()
{
	mPrimitiveWriter.EndLine();
}

void ObjectsContext::WriteTokenSeparator(ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteTokenSeparator(inSeparate);
}

void ObjectsContext::WriteKeyword(const string& inKeyword)
{
	mPrimitiveWriter.WriteKeyword(inKeyword);
}

void ObjectsContext::WriteInteger(long long inIntegerToken,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteInteger(inIntegerToken,inSeparate);
}

void ObjectsContext::WriteDouble(double inDoubleToken,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteDouble(inDoubleToken,inSeparate);
}

static const std::string scObj = "obj";
ObjectIDType ObjectsContext::StartNewIndirectObject()
{
	ObjectIDType newObjectID = mReferencesRegistry.AllocateNewObjectID();
	mReferencesRegistry.MarkObjectAsWritten(newObjectID,mOutputStream->GetCurrentPosition());
	mPrimitiveWriter.WriteInteger(newObjectID);
	mPrimitiveWriter.WriteInteger(0);
	mPrimitiveWriter.WriteKeyword(scObj);
	return newObjectID;
}

void ObjectsContext::StartNewIndirectObject(ObjectIDType inObjectID)
{
	mReferencesRegistry.MarkObjectAsWritten(inObjectID,mOutputStream->GetCurrentPosition());
	mPrimitiveWriter.WriteInteger(inObjectID);
	mPrimitiveWriter.WriteInteger(0);
	mPrimitiveWriter.WriteKeyword(scObj);
}

static const std::string scEndObj = "endobj";
void ObjectsContext::EndIndirectObject()
{
	mPrimitiveWriter.WriteKeyword(scEndObj);
}

void ObjectsContext::StartArray()
{
	mPrimitiveWriter.StartArray();
}

void ObjectsContext::EndArray(ETokenSeparator inSeparate)
{
	mPrimitiveWriter.EndArray(inSeparate);
}


static const string scLength = "Length";
static const string scStream = "stream";
static const string scEndStream = "endstream";
EStatusCode ObjectsContext::WritePDFStream(PDFStream* inStream)
{
	// move stream from "write to" mode to "read". Should make read stream available. also, now GetLength should provide a valid value
	EStatusCode status = inStream->FinalizeWriteAndStratRead();

	do
	{
		if(status != eSuccess)
		{
			TRACE_LOG("ObjectsContext::WriteStream. Error in converting PDF Stream from write mode to read mode");
			break;
		}

		// Write Stream Dictionary
		DictionaryContext* streamDictionaryContext = StartDictionary();

		// Write Length
		streamDictionaryContext->WriteKey(scLength);
		streamDictionaryContext->WriteIntegerValue(inStream->GetLength());
		
		EndDictionary(streamDictionaryContext);

		// Write Stream Content
		WriteKeyword(scStream);

		OutputStreamTraits copyTraits(mOutputStream);

		status = copyTraits.CopyToOutputStream(inStream->GetReadStream());
		if(status != eSuccess)
		{
			TRACE_LOG("ObjectsContext::WriteStream. Unable to write PDFStream to PDF file");
			break;
		}

		EndLine(); // this one just to make sure
		WriteKeyword(scEndStream);
	}while(false);

	return status;
}