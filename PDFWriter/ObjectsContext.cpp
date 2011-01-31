/*
   Source File : ObjectsContext.cpp


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
	mCompressStreams = true;
	mExtender = NULL;
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

void ObjectsContext::WriteHexString(const string& inString,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteHexString(inString,inSeparate);
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

void ObjectsContext::WriteBoolean(bool inBooleanToken,ETokenSeparator inSeparate)
{
	mPrimitiveWriter.WriteBoolean(inBooleanToken,inSeparate);
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

void ObjectsContext::SetCompressStreams(bool inCompressStreams)
{
	mCompressStreams = inCompressStreams;
}

static const string scLength = "Length";
static const string scStream = "stream";
static const string scEndStream = "endstream";
static const string scFilter = "Filter";
static const string scFlateDecode = "FlateDecode";

PDFStream* ObjectsContext::StartPDFStream(DictionaryContext* inStreamDictionary)
{
	// write stream header and allocate PDF stream.
	// PDF stream will take care of maintaining state for the stream till writing is finished

	// Write the stream header
	// Write Stream Dictionary (note that inStreamDictionary is optionally used)
	DictionaryContext* streamDictionaryContext = (NULL == inStreamDictionary ? StartDictionary() : inStreamDictionary);

	// Length (write as an indirect object)
	streamDictionaryContext->WriteKey(scLength);
	ObjectIDType lengthObjectID = mReferencesRegistry.AllocateNewObjectID();
	streamDictionaryContext->WriteObjectReferenceValue(lengthObjectID);
		
	// Compression (if necessary)
	if(mCompressStreams)
	{
		streamDictionaryContext->WriteKey(scFilter);
		streamDictionaryContext->WriteNameValue(scFlateDecode);
	}

	EndDictionary(streamDictionaryContext);

	// Write Stream Content
	WriteKeyword(scStream);

	// now begin the stream itself
	return new PDFStream(mCompressStreams,mOutputStream,lengthObjectID,mExtender);
}

PDFStream* ObjectsContext::StartUnfilteredPDFStream(DictionaryContext* inStreamDictionary)
{
	// write stream header and allocate PDF stream.
	// PDF stream will take care of maintaining state for the stream till writing is finished

	// Write the stream header
	// Write Stream Dictionary (note that inStreamDictionary is optionally used)
	DictionaryContext* streamDictionaryContext = (NULL == inStreamDictionary ? StartDictionary() : inStreamDictionary);

	// Length (write as an indirect object)
	streamDictionaryContext->WriteKey(scLength);
	ObjectIDType lengthObjectID = mReferencesRegistry.AllocateNewObjectID();
	streamDictionaryContext->WriteObjectReferenceValue(lengthObjectID);
		
	EndDictionary(streamDictionaryContext);

	// Write Stream Content
	WriteKeyword(scStream);

	// now begin the stream itself
	return new PDFStream(false,mOutputStream,lengthObjectID,NULL);
}

void ObjectsContext::EndPDFStream(PDFStream* inStream)
{
	// finalize the stream write to end stream context and calculate length
	inStream->FinalizeStreamWrite();
	WritePDFStreamEndWithoutExtent();
	EndIndirectObject();
	WritePDFStreamExtent(inStream);
}
	
void ObjectsContext::WritePDFStreamEndWithoutExtent()
{
		EndLine(); // this one just to make sure
		WriteKeyword(scEndStream);
}

void ObjectsContext::WritePDFStreamExtent(PDFStream* inStream)
{
	StartNewIndirectObject(inStream->GetExtentObjectID());
	WriteInteger(inStream->GetLength(),eTokenSeparatorEndLine);
	EndIndirectObject();
}

void ObjectsContext::SetObjectsContextExtender(IObjectsContextExtender* inExtender)
{
	mExtender = inExtender;
}

string ObjectsContext::GenerateSubsetFontPrefix()
{
	return mSubsetFontsNamesSequance.GetNextValue();
}