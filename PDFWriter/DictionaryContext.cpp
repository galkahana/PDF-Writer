#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "Trace.h"
#include "IByteWriterWithPosition.h"

static const string scStartDictionary = "<<";
static const string scEndDictionary = ">>";

DictionaryContext::DictionaryContext(ObjectsContext* inObjectsContext,size_t inIndentLevel)
{
	mObjectsContext = inObjectsContext;
	mIndentLevel= inIndentLevel;

	mObjectsContext->WriteKeyword(scStartDictionary);
}

DictionaryContext::~DictionaryContext(void)
{
	if(mIndentLevel > 0)
	{
		--mIndentLevel; // the final end dictionary should be written with a lower indent, as a value of the container
		WriteIndents();
	}
	mObjectsContext->WriteKeyword(scEndDictionary);
}

EStatusCode DictionaryContext::WriteKey(const string& inKey)
{
	if(mKeys.find(inKey) == mKeys.end())
	{
		WriteIndents();
		mObjectsContext->WriteName(inKey);
		mKeys.insert(inKey);
		return eSuccess;
	}
	else
	{
		TRACE_LOG1("DictionaryContext::WriteKey, Duplicate key error. Cannot write multiple keys in the same dictionary. key reused - %s",inKey.c_str());
		return eFailure;
	}
}

static const Byte scTab[1] = {'\t'};
void DictionaryContext::WriteIndents()
{
	IByteWriterWithPosition* outputStream = mObjectsContext->StartFreeContext();
	for(size_t i=0;i<=mIndentLevel;++i)
		outputStream->Write(scTab,1);
	mObjectsContext->EndFreeContext();
}

void DictionaryContext::WriteIntegerValue(long long inValue)
{
	mObjectsContext->WriteInteger(inValue,eTokenSeparatorEndLine);
}

void DictionaryContext::WriteObjectReferenceValue(ObjectIDType inObjectReference)
{
	mObjectsContext->WriteIndirectObjectReference(inObjectReference,eTokenSeparatorEndLine);
}

void DictionaryContext::WriteLiteralStringValue(const string& inValue)
{
	mObjectsContext->WriteLiteralString(inValue,eTokenSeparatorEndLine);
}

void DictionaryContext::WriteNameValue(const string& inValue)
{
	mObjectsContext->WriteName(inValue,eTokenSeparatorEndLine);	
}

void DictionaryContext::WriteRectangleValue(const PDFRectangle& inRectangle)
{
	mObjectsContext->StartArray();
	mObjectsContext->WriteDouble(inRectangle.LowerLeftX);
	mObjectsContext->WriteDouble(inRectangle.LowerLeftY);
	mObjectsContext->WriteDouble(inRectangle.UpperRightX);
	mObjectsContext->WriteDouble(inRectangle.UpperRightY);
	mObjectsContext->EndArray();
	mObjectsContext->EndLine();
}

