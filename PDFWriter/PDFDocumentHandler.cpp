/*
   Source File : PDFDocumentHandler.cpp


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
#include "PDFDocumentHandler.h"
#include "Trace.h"
#include "RefCountPtr.h"
#include "PDFObjectCast.h"
#include "PDFArray.h"
#include "PDFInteger.h"
#include "PDFReal.h"
#include "PDFDictionary.h"
#include "DocumentContext.h"
#include "PDFFormXObject.h"
#include "PDFStream.h"
#include "OutputStreamTraits.h"
#include "PDFStreamInput.h"
#include "InputFlateDecodeStream.h"
#include "ObjectsContext.h"
#include "PDFIndirectObjectReference.h"
#include "PDFBoolean.h"
#include "PDFSymbol.h"
#include "DictionaryContext.h"
#include "PDFLiteralString.h"
#include "PDFHexString.h"
#include "PrimitiveObjectsWriter.h"
#include "PageContentContext.h"
#include "PDFPage.h"


PDFDocumentHandler::PDFDocumentHandler(void)
{
	mObjectsContext = NULL;
	mDocumentContext = NULL;
	mWrittenPage = NULL;
}

PDFDocumentHandler::~PDFDocumentHandler(void)
{
}

void PDFDocumentHandler::SetOperationsContexts(DocumentContext* inDocumentContext,ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
	mDocumentContext = inDocumentContext;
}

class IPageEmbedInFormCommand
{
public:

	virtual PDFFormXObject* CreatePDFFormXObjectForPage(PDFDocumentHandler* inDocumentHandler,unsigned long i,const double* inTransformationMatrix) = 0;
};

class PageEmbedInFormWithCropBox: public IPageEmbedInFormCommand
{
public:
	PageEmbedInFormWithCropBox(const PDFRectangle& inCropBox)
	{
		mCropBox = inCropBox;
	}

	PDFFormXObject* CreatePDFFormXObjectForPage(PDFDocumentHandler* inDocumentHandler,unsigned long i,const double* inTransformationMatrix)
	{
		return inDocumentHandler->CreatePDFFormXObjectForPage(i,mCropBox,inTransformationMatrix);
	}

private:
	PDFRectangle mCropBox;

};

class PageEmbedInFormWithPageBox: public IPageEmbedInFormCommand
{
public:
	PageEmbedInFormWithPageBox(EPDFPageBox inPageBoxToUseAsFormBox)
	{
		mPageBoxToUseAsFormBox = inPageBoxToUseAsFormBox;
	}

	PDFFormXObject* CreatePDFFormXObjectForPage(PDFDocumentHandler* inDocumentHandler,unsigned long i,const double* inTransformationMatrix)
	{
		return inDocumentHandler->CreatePDFFormXObjectForPage(i,mPageBoxToUseAsFormBox,inTransformationMatrix);
	}

private:
	EPDFPageBox mPageBoxToUseAsFormBox;

};


EStatusCodeAndObjectIDTypeList PDFDocumentHandler::CreateFormXObjectsFromPDF(	const wstring& inPDFFilePath,
																				const PDFPageRange& inPageRange,
																				IPageEmbedInFormCommand* inPageEmbedCommand,
																				const double* inTransformationMatrix,
																				const ObjectIDTypeList& inCopyAdditionalObjects)
{
	EStatusCodeAndObjectIDTypeList result;

	do
	{
		result.first = StartFileCopyingContext(inPDFFilePath);
		if(result.first != eSuccess)
			break;

		IDocumentContextExtenderSet::iterator it = mExtenders.begin();
		for(; it != mExtenders.end() && eSuccess == result.first; ++it)
		{
			result.first = (*it)->OnPDFParsingComplete(mObjectsContext,mDocumentContext,this);
			if(result.first != eSuccess)
				TRACE_LOG("DocumentContext::CreateFormXObjectsFromPDF, unexpected failure. extender declared failure after parsing page.");
		}

		// copy additional objects prior to pages, so we have them ready at page copying
		if(inCopyAdditionalObjects.size() > 0)
		{
			result.first = WriteNewObjects(inCopyAdditionalObjects);
			if(result.first != eSuccess)
			{
				TRACE_LOG("PDFDocumentHandler::CreateFormXObjectsFromPDF, failed copying additional objects");
				break;
			}
		}

		PDFFormXObject* newObject;

		if(PDFPageRange::eRangeTypeAll == inPageRange.mType)
		{
			for(unsigned long i=0; i < mParser.GetPagesCount() && eSuccess == result.first; ++i)
			{
				newObject = inPageEmbedCommand->CreatePDFFormXObjectForPage(this,i,inTransformationMatrix);
				if(newObject)
				{
					result.second.push_back(newObject->GetObjectID());
					delete newObject;
				}
				else
				{
					TRACE_LOG1("PDFDocumentHandler::CreateFormXObjectsFromPDF, failed to embed page %ld", i);
					result.first = eFailure;
				}
			}
		}
		else
		{
			// eRangeTypeSpecific
			ULongAndULongList::const_iterator it = inPageRange.mSpecificRanges.begin();
			for(; it != inPageRange.mSpecificRanges.end() && eSuccess == result.first;++it)
			{
				if(it->first <= it->second && it->second < mParser.GetPagesCount())
				{
					for(unsigned long i=it->first; i <= it->second && eSuccess == result.first; ++i)
					{
						newObject = inPageEmbedCommand->CreatePDFFormXObjectForPage(this,i,inTransformationMatrix);
						if(newObject)
						{
							result.second.push_back(newObject->GetObjectID());
							delete newObject;
						}
						else
						{
							TRACE_LOG1("PDFDocumentHandler::CreateFormXObjectsFromPDF, failed to embed page %ld", i);
							result.first = eFailure;
						}
					}
				}
				else
				{
					TRACE_LOG3("PDFDocumentHandler::CreateFormXObjectsFromPDF, range mismatch. first = %ld, second = %ld, PDF page count = %ld", 
						it->first,
						it->second,
						mParser.GetPagesCount());
					result.first = eFailure;
				}
			}
		}


	}while(false);

	IDocumentContextExtenderSet::iterator it = mExtenders.begin();
	for(; it != mExtenders.end() && eSuccess == result.first; ++it)
	{
		result.first = (*it)->OnPDFCopyingComplete(mObjectsContext,mDocumentContext,this);
		if(result.first != eSuccess)
			TRACE_LOG("DocumentContext::CreateFormXObjectsFromPDF, unexpected failure. extender declared failure before finalizing copy.");
	}


	StopFileCopyingContext();

	return result;

}

EStatusCodeAndObjectIDTypeList PDFDocumentHandler::CreateFormXObjectsFromPDF(	const wstring& inPDFFilePath,
																				const PDFPageRange& inPageRange,
																				EPDFPageBox inPageBoxToUseAsFormBox,
																				const double* inTransformationMatrix,
																				const ObjectIDTypeList& inCopyAdditionalObjects)
{
	PageEmbedInFormWithPageBox embedCommand(inPageBoxToUseAsFormBox);
	return CreateFormXObjectsFromPDF(inPDFFilePath,inPageRange,&embedCommand,inTransformationMatrix,inCopyAdditionalObjects);
}

EStatusCodeAndObjectIDTypeList PDFDocumentHandler::CreateFormXObjectsFromPDF(	const wstring& inPDFFilePath,
																				const PDFPageRange& inPageRange,
																				const PDFRectangle& inCropBox,
																				const double* inTransformationMatrix,
																				const ObjectIDTypeList& inCopyAdditionalObjects)
{
	PageEmbedInFormWithCropBox embedCommand(inCropBox);
	return CreateFormXObjectsFromPDF(inPDFFilePath,inPageRange,&embedCommand,inTransformationMatrix,inCopyAdditionalObjects);
}

PDFFormXObject* PDFDocumentHandler::CreatePDFFormXObjectForPage(unsigned long inPageIndex,
																EPDFPageBox inPageBoxToUseAsFormBox,
																const double* inTransformationMatrix)
{
	RefCountPtr<PDFDictionary> pageObject = mParser.ParsePage(inPageIndex);

	if(!pageObject)
	{
		TRACE_LOG1("PDFDocumentHandler::CreatePDFFormXObjectForPage, unhexpected exception, page index does not denote a page object. page index = %ld",inPageIndex);
		return NULL;
	}
	else
		return CreatePDFFormXObjectForPage(pageObject.GetPtr(),DeterminePageBox(pageObject.GetPtr(),inPageBoxToUseAsFormBox),inTransformationMatrix);
}

PDFFormXObject* PDFDocumentHandler::CreatePDFFormXObjectForPage(PDFDictionary* inPageObject,
																const PDFRectangle& inFormBox,
																const double* inTransformationMatrix)
{
	PDFFormXObject* result = NULL;
	EStatusCode status = eSuccess;

	IDocumentContextExtenderSet::iterator it = mExtenders.begin();
	for(; it != mExtenders.end() && eSuccess == status; ++it)
	{
		status = (*it)->OnBeforeCreateXObjectFromPage(inPageObject,mObjectsContext,mDocumentContext,this);
		if(status != eSuccess)
			TRACE_LOG("DocumentContext::CreatePDFFormXObjectForPage, unexpected failure. extender declared failure before writing page.");
	}
	if(status != eSuccess)
		return NULL;

	do
	{
		if(CopyResourcesIndirectObjects(inPageObject) != eSuccess)
			break;

		// Create a new form XObject
		result = mDocumentContext->StartFormXObject(inFormBox,inTransformationMatrix);

		// copy the page content to the target XObject stream
		if(WritePageContentToSingleStream(result->GetContentStream()->GetWriteStream(),inPageObject) != eSuccess)
		{
			delete result;
			result = NULL;
			break;
		}

		// resources dictionary is gonna be empty at this point...so we can use our own code to write the dictionary, by extending.
		// which will be a simple loop. note that at this point all indirect objects should have been copied, and have mapping
		mDocumentContext->AddDocumentContextExtender(this);
		mWrittenPage = inPageObject;

		if(mDocumentContext->EndFormXObjectNoRelease(result) != eSuccess)
		{
			delete result;
			result = NULL;
			break;
		}

	}while(false);

	mWrittenPage = NULL;
	mDocumentContext->RemoveDocumentContextExtender(this);

	if(result)
	{
		IDocumentContextExtenderSet::iterator it = mExtenders.begin();
		for(; it != mExtenders.end() && eSuccess == status; ++it)
		{
			status = (*it)->OnAfterCreateXObjectFromPage(result,inPageObject,mObjectsContext,mDocumentContext,this);
			if(status != eSuccess)
				TRACE_LOG("DocumentContext::CreatePDFFormXObjectForPage, unexpected failure. extender declared failure after writing page.");
		}
		if(status != eSuccess)
		{
			delete result;
			return NULL;
		}
		else
			return result;
	}
	else
		return result;
}

PDFRectangle PDFDocumentHandler::DeterminePageBox(PDFDictionary* inDictionary,EPDFPageBox inPageBoxType)
{
	PDFRectangle result;

	switch(inPageBoxType)
	{
		case ePDFPageBoxMediaBox:
		{
			PDFObjectCastPtr<PDFArray> mediaBox(QueryInheritedValue(inDictionary,"MediaBox"));
			if(!mediaBox || mediaBox->GetLength() != 4)
			{
				TRACE_LOG("PDFDocumentHandler::DeterminePageBox, Exception, pdf page does not have correct media box. defaulting to A4");
				result = PDFRectangle(0,0,595,842);
			}
			else
			{
				SetPDFRectangleFromPDFArray(mediaBox.GetPtr(),result);
			}
			break;
		}
		case ePDFPageBoxCropBox:
		{
			PDFObjectCastPtr<PDFArray> cropBox(QueryInheritedValue(inDictionary,"CropBox"));
			
			if(!cropBox || cropBox->GetLength() != 4)
			{
				TRACE_LOG("PDFDocumentHandler::DeterminePageBox, PDF does not have crop box, defaulting to media box.");
				result = DeterminePageBox(inDictionary,ePDFPageBoxMediaBox);
			}
			else
			{
				SetPDFRectangleFromPDFArray(cropBox.GetPtr(),result);
			}
			break;
		}
		case ePDFPageBoxBleedBox:
		{
			PDFObjectCastPtr<PDFArray> bleedBox(mParser.QueryDictionaryObject(inDictionary,"BleedBox"));
			if(!bleedBox || bleedBox->GetLength() != 4)
			{
				TRACE_LOG("PDFDocumentHandler::DeterminePageBox, PDF does not have bleed box, defaulting to crop box.");
				result = DeterminePageBox(inDictionary,ePDFPageBoxCropBox);
			}
			else
			{
				SetPDFRectangleFromPDFArray(bleedBox.GetPtr(),result);
			}
			break;
		}
		case ePDFPageBoxTrimBox:
		{
			PDFObjectCastPtr<PDFArray> trimBox(mParser.QueryDictionaryObject(inDictionary,"TrimBox"));
			if(!trimBox || trimBox->GetLength() != 4)
			{
				TRACE_LOG("PDFDocumentHandler::DeterminePageBox, PDF does not have trim box, defaulting to crop box.");
				result = DeterminePageBox(inDictionary,ePDFPageBoxCropBox);
			}
			else
			{
				SetPDFRectangleFromPDFArray(trimBox.GetPtr(),result);
			}
			break;
		}
		case ePDFPageBoxArtBox:
		{
			PDFObjectCastPtr<PDFArray> artBox(mParser.QueryDictionaryObject(inDictionary,"ArtBox"));
			if(!artBox || artBox->GetLength() != 4)
			{
				TRACE_LOG("PDFDocumentHandler::DeterminePageBox, PDF does not have art box, defaulting to crop box.");
				result = DeterminePageBox(inDictionary,ePDFPageBoxCropBox);
			}
			else
			{
				SetPDFRectangleFromPDFArray(artBox.GetPtr(),result);
			}
			break;
		}
	}
	return result;
}

void PDFDocumentHandler::SetPDFRectangleFromPDFArray(PDFArray* inPDFArray,PDFRectangle& outPDFRectangle)
{
	RefCountPtr<PDFObject> lowerLeftX(inPDFArray->QueryObject(0));
	RefCountPtr<PDFObject> lowerLeftY(inPDFArray->QueryObject(1));
	RefCountPtr<PDFObject> upperRightX(inPDFArray->QueryObject(2));
	RefCountPtr<PDFObject> upperRightY(inPDFArray->QueryObject(3));
	
	outPDFRectangle.LowerLeftX = GetAsDoubleValue(lowerLeftX.GetPtr());
	outPDFRectangle.LowerLeftY = GetAsDoubleValue(lowerLeftY.GetPtr());
	outPDFRectangle.UpperRightX = GetAsDoubleValue(upperRightX.GetPtr());
	outPDFRectangle.UpperRightY = GetAsDoubleValue(upperRightY.GetPtr());
}

double PDFDocumentHandler::GetAsDoubleValue(PDFObject* inNumberObject)
{
	if(inNumberObject->GetType() == ePDFObjectInteger)
	{
		PDFInteger* anInteger = (PDFInteger*)inNumberObject;
		return (double)anInteger->GetValue();
	}
	else if(inNumberObject->GetType() == ePDFObjectReal)
	{
		PDFReal* aReal = (PDFReal*)inNumberObject;
		return aReal->GetValue();
	}
	else
		return 0;
}

PDFFormXObject* PDFDocumentHandler::CreatePDFFormXObjectForPage(unsigned long inPageIndex,
																const PDFRectangle& inCropBox,
																const double* inTransformationMatrix)
{
	RefCountPtr<PDFDictionary> pageObject = mParser.ParsePage(inPageIndex);

	if(!pageObject)
	{
		TRACE_LOG1("PDFDocumentHandler::CreatePDFFormXObjectForPage, unhexpected exception, page index does not denote a page object. page index = %ld",inPageIndex);
		return NULL;
	}
	else
		return CreatePDFFormXObjectForPage(pageObject.GetPtr(),inCropBox,inTransformationMatrix);
}

EStatusCode PDFDocumentHandler::WritePageContentToSingleStream(IByteWriter* inTargetStream,PDFDictionary* inPageObject)
{
	EStatusCode status = eSuccess;

	RefCountPtr<PDFObject> pageContent(mParser.QueryDictionaryObject(inPageObject,"Contents"));
	if(pageContent->GetType() == ePDFObjectStream)
	{
		status = WritePDFStreamInputToStream(inTargetStream,(PDFStreamInput*)pageContent.GetPtr());
	}
	else if(pageContent->GetType() == ePDFObjectArray)
	{
		SingleValueContainerIterator<PDFObjectVector> it = ((PDFArray*)pageContent.GetPtr())->GetIterator();
		PDFObjectCastPtr<PDFIndirectObjectReference> refItem;
		while(it.MoveNext() && status == eSuccess)
		{
			refItem = it.GetItem();
			if(!refItem)
			{
				status = eFailure;
				TRACE_LOG("PDFDocumentHandler::WritePageContentToSingleStream, content stream array contains non-refs");
				break;
			}
			PDFObjectCastPtr<PDFStreamInput> contentStream(mParser.ParseNewObject(refItem->mObjectID));
			if(!contentStream)
			{
				status = eFailure;
				TRACE_LOG("PDFDocumentHandler::WritePageContentToSingleStream, content stream array contains references to non streams");
				break;
			}
			status = WritePDFStreamInputToStream(inTargetStream,contentStream.GetPtr());
			if(eSuccess == status)
			{
				// separate the streams with a nice endline
				PrimitiveObjectsWriter primitivesWriter;
				primitivesWriter.SetStreamForWriting(inTargetStream);
				primitivesWriter.EndLine();
			}
		}
	}
	else
	{
		TRACE_LOG1("PDFDocumentHandler::WritePageContentToSingleStream, error copying page content, expected either array or stream, getting %s",scPDFObjectTypeLabel[pageContent->GetType()]);
		status = eFailure;
	}
	

	return status;
}

EStatusCode PDFDocumentHandler::WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream)
{
	IByteReader* streamReader = mParser.CreateInputStreamReader(inSourceStream);

	if(!streamReader)
		return eFailure;

	mPDFFile.GetInputStream()->SetPosition(inSourceStream->GetStreamContentStart());

	OutputStreamTraits traits(inTargetStream);
	EStatusCode status = traits.CopyToOutputStream(streamReader);
	delete streamReader;
	return status;
}

EStatusCode PDFDocumentHandler::CopyResourcesIndirectObjects(PDFDictionary* inPage)
{
	// makes sure that all indirect references are copied. those will come from the resources dictionary.
	// this is how we go about this:
	// Loop the immediate entities of the resources dictionary. for each value (which may be indirect) do this:
	// if indirect, run CopyInDirectObject on it (passing its ID and a new ID at the target PDF (just allocate as you go))
	// if direct, let go.

	PDFObjectCastPtr<PDFDictionary> resources(mParser.QueryDictionaryObject(inPage,"Resources"));

	// k. no resources...as wierd as that might be...or just wrong...i'll let it be
	if(!resources)
		return eSuccess;

	ObjectIDTypeList newObjectsToWrite;

	RegisterInDirectObjects(resources.GetPtr(),newObjectsToWrite);
	return WriteNewObjects(newObjectsToWrite);
}

EStatusCode PDFDocumentHandler::WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs)
{
	// notice that using this method directly is slightly unsafe - the input objects are ASSUMED to have not been
	// copied yet. unexpected results if the objects of these ids were already copied.

	ObjectIDTypeSet writtenObjects;
	return WriteNewObjects(inSourceObjectIDs,writtenObjects);
}


EStatusCode PDFDocumentHandler::WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs,ObjectIDTypeSet& ioCopiedObjects)
{

	ObjectIDTypeList::const_iterator itNewObjects = inSourceObjectIDs.begin();
	EStatusCode status = eSuccess;

	for(; itNewObjects != inSourceObjectIDs.end() && eSuccess == status; ++itNewObjects)
	{
		// theoretically speaking, it could be that while one object was copied, another one in this array is already
		// copied, so make sure to check that these objects are still required for copying
		if(ioCopiedObjects.find(*itNewObjects) == ioCopiedObjects.end())
		{
			ObjectIDTypeToObjectIDTypeMap::iterator it = mSourceToTarget.find(*itNewObjects);
			if(it == mSourceToTarget.end())
			{
				ObjectIDType newObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
				it = mSourceToTarget.insert(ObjectIDTypeToObjectIDTypeMap::value_type(*itNewObjects,newObjectID)).first;
			}
			ioCopiedObjects.insert(*itNewObjects);
			status = CopyInDirectObject(*itNewObjects,it->second,ioCopiedObjects);
		}
	}
	return status;
}

void PDFDocumentHandler::RegisterInDirectObjects(PDFDictionary* inDictionary,ObjectIDTypeList& outNewObjects)
{
	MapIterator<PDFNameToPDFObjectMap> it(inDictionary->GetIterator());

	// i'm assuming keys are directs. i can move into indirects if that's important
	while(it.MoveNext())
	{
		if(it.GetValue()->GetType() == ePDFObjectIndirectObjectReference)
		{
			ObjectIDTypeToObjectIDTypeMap::iterator	itObjects = mSourceToTarget.find(((PDFIndirectObjectReference*)it.GetValue())->mObjectID);
			if(itObjects == mSourceToTarget.end())
				outNewObjects.push_back(((PDFIndirectObjectReference*)it.GetValue())->mObjectID);
		} 
		else if(it.GetValue()->GetType() == ePDFObjectArray)
		{
			RegisterInDirectObjects((PDFArray*)it.GetValue(),outNewObjects);
		}
		else if(it.GetValue()->GetType() == ePDFObjectDictionary)
		{
			RegisterInDirectObjects((PDFDictionary*)it.GetValue(),outNewObjects);
		}
	}
}

void PDFDocumentHandler::RegisterInDirectObjects(PDFArray* inArray,ObjectIDTypeList& outNewObjects)
{
	SingleValueContainerIterator<PDFObjectVector> it(inArray->GetIterator());

	while(it.MoveNext())
	{
		if(it.GetItem()->GetType() == ePDFObjectIndirectObjectReference)
		{
			ObjectIDTypeToObjectIDTypeMap::iterator	itObjects = mSourceToTarget.find(((PDFIndirectObjectReference*)it.GetItem())->mObjectID);
			if(itObjects == mSourceToTarget.end())
				outNewObjects.push_back(((PDFIndirectObjectReference*)it.GetItem())->mObjectID);
		} 
		else if(it.GetItem()->GetType() == ePDFObjectArray)
		{
			RegisterInDirectObjects((PDFArray*)it.GetItem(),outNewObjects);
		}
		else if(it.GetItem()->GetType() == ePDFObjectDictionary)
		{
			RegisterInDirectObjects((PDFDictionary*)it.GetItem(),outNewObjects);
		}
	}
}

EStatusCode PDFDocumentHandler::CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID,ObjectIDTypeSet& ioCopiedObjects)
{
	// CopyInDirectObject will do this (lissen up)
	// Start a new object with the input ID
	// Write the object according to type. For arrays/dicts there might be indirect objects to copy. for them do this:
	// if you got it in the map already, just write down the new ID. if not register the ID, with a new ID already allocated at this point.
	// Once done. loop what you registered, using CopyInDirectObject in the same manner. This should maintain that each object is written separately
	EStatusCode status;
	ObjectIDTypeList newObjectsToWrite;

	RefCountPtr<PDFObject> sourceObject = mParser.ParseNewObject(inSourceObjectID);
	if(!sourceObject)
	{
		TRACE_LOG1("PDFDocumentHandler::CopyInDirectObject, object not found. %ld",inSourceObjectID);
		return eFailure;
	}

	mObjectsContext->StartNewIndirectObject(inTargetObjectID);
	status = WriteObjectByType(sourceObject.GetPtr(),eTokenSeparatorEndLine,newObjectsToWrite);
	if(eSuccess == status)
	{
		mObjectsContext->EndIndirectObject();
		return WriteNewObjects(newObjectsToWrite,ioCopiedObjects);
	}
	else
		return status;
}

EStatusCode PDFDocumentHandler::WriteObjectByType(PDFObject* inObject,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd)
{
	EStatusCode status = eSuccess;

	switch(inObject->GetType())
	{
		case ePDFObjectBoolean:
		{
			mObjectsContext->WriteBoolean(((PDFBoolean*)inObject)->GetValue(),inSeparator);
			break;
		}
		case ePDFObjectLiteralString:
		{
			mObjectsContext->WriteLiteralString(((PDFLiteralString*)inObject)->GetValue(),inSeparator);
			break;
		}
		case ePDFObjectHexString:
		{
			mObjectsContext->WriteHexString(((PDFHexString*)inObject)->GetValue(),inSeparator);
			break;
		}
		case ePDFObjectNull:
		{
			mObjectsContext->WriteNull(eTokenSeparatorEndLine);
			break;
		}
		case ePDFObjectName:
		{
			mObjectsContext->WriteName(((PDFName*)inObject)->GetValue(),inSeparator);
			break;
		}
		case ePDFObjectInteger:
		{
			mObjectsContext->WriteInteger(((PDFInteger*)inObject)->GetValue(),inSeparator);
			break;
		}
		case ePDFObjectReal:
		{
			mObjectsContext->WriteDouble(((PDFReal*)inObject)->GetValue(),inSeparator);
			break;
		}
		case ePDFObjectSymbol:
		{
			mObjectsContext->WriteKeyword(((PDFSymbol*)inObject)->GetValue());
			break;
		}
		case ePDFObjectIndirectObjectReference:
		{
			ObjectIDType sourceObjectID = ((PDFIndirectObjectReference*)inObject)->mObjectID;
			ObjectIDTypeToObjectIDTypeMap::iterator	itObjects = mSourceToTarget.find(sourceObjectID);
			if(itObjects == mSourceToTarget.end())
			{
				ObjectIDType newObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
				itObjects = mSourceToTarget.insert(ObjectIDTypeToObjectIDTypeMap::value_type(sourceObjectID,newObjectID)).first;
				outSourceObjectsToAdd.push_back(sourceObjectID);
			}
			mObjectsContext->WriteIndirectObjectReference(itObjects->second,inSeparator);
			break;
		}
		case ePDFObjectArray:
		{
			status = WriteArrayObject((PDFArray*)inObject,inSeparator,outSourceObjectsToAdd);
			break;
		}
		case ePDFObjectDictionary:
		{
			status = WriteDictionaryObject((PDFDictionary*)inObject,outSourceObjectsToAdd);
			break;
		}
		case ePDFObjectStream:
		{
			status = WriteStreamObject((PDFStreamInput*)inObject,outSourceObjectsToAdd);
			break;
		}
	}
	return status;
}


EStatusCode PDFDocumentHandler::WriteArrayObject(PDFArray* inArray,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd)
{
	SingleValueContainerIterator<PDFObjectVector> it(inArray->GetIterator());

	EStatusCode status = eSuccess;
	
	mObjectsContext->StartArray();

	while(it.MoveNext() && eSuccess == status)
		status = WriteObjectByType(it.GetItem(),eTokenSeparatorSpace,outSourceObjectsToAdd);

	if(eSuccess == status)
		mObjectsContext->EndArray(inSeparator);

	return status;
}



EStatusCode PDFDocumentHandler::WriteDictionaryObject(PDFDictionary* inDictionary,ObjectIDTypeList& outSourceObjectsToAdd)
{
	MapIterator<PDFNameToPDFObjectMap> it(inDictionary->GetIterator());
	EStatusCode status = eSuccess;
	DictionaryContext* dictionary = mObjectsContext->StartDictionary();

	while(it.MoveNext() && eSuccess == status)
	{
		status = dictionary->WriteKey(it.GetKey()->GetValue());
		if(eSuccess == status)
			status = WriteObjectByType(it.GetValue(),dictionary,outSourceObjectsToAdd);
	}
	
	if(eSuccess == status)
	{
		return mObjectsContext->EndDictionary(dictionary);
	}
	else
		return eSuccess;
}

EStatusCode PDFDocumentHandler::WriteObjectByType(PDFObject* inObject,DictionaryContext* inDictionaryContext,ObjectIDTypeList& outSourceObjectsToAdd)
{
	EStatusCode status = eSuccess;

	switch(inObject->GetType())
	{
		case ePDFObjectBoolean:
		{
			inDictionaryContext->WriteBooleanValue(((PDFBoolean*)inObject)->GetValue());
			break;
		}
		case ePDFObjectLiteralString:
		{
			inDictionaryContext->WriteLiteralStringValue(((PDFLiteralString*)inObject)->GetValue());
			break;
		}
		case ePDFObjectHexString:
		{
			inDictionaryContext->WriteHexStringValue(((PDFHexString*)inObject)->GetValue());
			break;
		}
		case ePDFObjectNull:
		{
			inDictionaryContext->WriteNullValue();
			break;
		}
		case ePDFObjectName:
		{
			inDictionaryContext->WriteNameValue(((PDFName*)inObject)->GetValue());
			break;
		}
		case ePDFObjectInteger:
		{
			inDictionaryContext->WriteIntegerValue(((PDFInteger*)inObject)->GetValue());
			break;
		}
		case ePDFObjectReal:
		{
			inDictionaryContext->WriteDoubleValue(((PDFReal*)inObject)->GetValue());
			break;
		}
		case ePDFObjectSymbol:
		{
			// not sure this is supposed to happen...but then...it is probably not supposed to happen at any times...
			inDictionaryContext->WriteKeywordValue(((PDFSymbol*)inObject)->GetValue());
			break;
		}
		case ePDFObjectIndirectObjectReference:
		{
			ObjectIDType sourceObjectID = ((PDFIndirectObjectReference*)inObject)->mObjectID;
			ObjectIDTypeToObjectIDTypeMap::iterator	itObjects = mSourceToTarget.find(sourceObjectID);
			if(itObjects == mSourceToTarget.end())
			{
				ObjectIDType newObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
				itObjects = mSourceToTarget.insert(ObjectIDTypeToObjectIDTypeMap::value_type(sourceObjectID,newObjectID)).first;
				outSourceObjectsToAdd.push_back(sourceObjectID);
			}
			inDictionaryContext->WriteObjectReferenceValue(itObjects->second);
			break;
		}
		case ePDFObjectArray:
		{
			status = WriteArrayObject((PDFArray*)inObject,eTokenSeparatorEndLine,outSourceObjectsToAdd);
			break;
		}
		case ePDFObjectDictionary:
		{
			status = WriteDictionaryObject((PDFDictionary*)inObject,outSourceObjectsToAdd);
			break;
		}
		case ePDFObjectStream:
		{
			// k. that's not supposed to happen
			TRACE_LOG("PDFDocumentHandler::WriteObjectByType, got that wrong sir. ain't gonna write a stream in a dictionary. must be a ref. we got exception here");
			break;
		}
	}
	return status;
}

EStatusCode PDFDocumentHandler::WriteStreamObject(PDFStreamInput* inStream,ObjectIDTypeList& outSourceObjectsToAdd)
{
	// i'm going to copy the stream directly, cause i don't need all this transcoding and such. if we ever do, i'll write a proper
	// PDFStream implementation.
	RefCountPtr<PDFDictionary> streamDictionary(inStream->QueryStreamDictionary());

	if(WriteDictionaryObject(streamDictionary.GetPtr(),outSourceObjectsToAdd) != eSuccess)
	{
		TRACE_LOG("PDFDocumentHandler::WriteStreamObject, failed to write stream dictionary");
		return eFailure;
	}

	mObjectsContext->WriteKeyword("stream");


	PDFObjectCastPtr<PDFInteger> lengthObject(mParser.QueryDictionaryObject(streamDictionary.GetPtr(),"Length"));	

	if(!lengthObject)
	{
		TRACE_LOG("PDFDocumentHandler::WriteStreamObject, stream does not have length, failing");
		return eFailure;
	}

	mPDFFile.GetInputStream()->SetPosition(inStream->GetStreamContentStart());

	OutputStreamTraits traits(mObjectsContext->StartFreeContext());
	EStatusCode status = traits.CopyToOutputStream(mPDFFile.GetInputStream(),(LongBufferSizeType)lengthObject->GetValue());	
	if(eSuccess == status)
	{
		mObjectsContext->EndFreeContext();
		mObjectsContext->EndLine(); // this one just to make sure
		mObjectsContext->WriteKeyword("endstream");
	}
	return status;
}

EStatusCode PDFDocumentHandler::OnResourcesWrite(
						ResourcesDictionary* inResources,
						DictionaryContext* inPageResourcesDictionaryContext,
						ObjectsContext* inPDFWriterObjectContext,
						DocumentContext* inPDFWriterDocumentContext)
{
	// Writing resources dictionary. simply loop internal elements and copy. nicely enough, i can use read methods, trusting
	// that no new objects need be written
	
	PDFObjectCastPtr<PDFDictionary> resources(mParser.QueryDictionaryObject(mWrittenPage,"Resources"));
	ObjectIDTypeList dummyObjectList; // this one should remain empty...

	// k. no resources...as wierd as that might be...or just wrong...i'll let it be
	if(!resources)
		return eSuccess;

	MapIterator<PDFNameToPDFObjectMap> it(resources->GetIterator());
	EStatusCode status = eSuccess;

	while(it.MoveNext() && eSuccess == status)
	{
		status = inPageResourcesDictionaryContext->WriteKey(it.GetKey()->GetValue());
		if(eSuccess == status)
			status = WriteObjectByType(it.GetValue(),inPageResourcesDictionaryContext,dummyObjectList);
	}
	return status;
}

EStatusCodeAndObjectIDTypeList PDFDocumentHandler::AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
																		const PDFPageRange& inPageRange,
																		const ObjectIDTypeList& inCopyAdditionalObjects)
{
	EStatusCodeAndObjectIDTypeList result;
	

	do
	{
		result.first = StartFileCopyingContext(inPDFFilePath);
		if(result.first != eSuccess)
			break;

		IDocumentContextExtenderSet::iterator it = mExtenders.begin();
		for(; it != mExtenders.end() && eSuccess == result.first; ++it)
		{
			result.first = (*it)->OnPDFParsingComplete(mObjectsContext,mDocumentContext,this);
			if(result.first != eSuccess)
				TRACE_LOG("DocumentContext::AppendPDFPagesFromPDF, unexpected failure. extender declared failure after parsing page.");
		}

		// copy additional objects prior to pages, so we have them ready at page copying
		if(inCopyAdditionalObjects.size() > 0)
		{
			result.first = WriteNewObjects(inCopyAdditionalObjects);
			if(result.first != eSuccess)
			{
				TRACE_LOG("PDFDocumentHandler::AppendPDFPagesFromPDF, failed copying additional objects");
				break;
			}
		}

		EStatusCodeAndObjectIDType newObject;

		if(PDFPageRange::eRangeTypeAll == inPageRange.mType)
		{
			for(unsigned long i=0; i < mParser.GetPagesCount() && eSuccess == result.first; ++i)
			{
				newObject = CreatePDFPageForPage(i);
				if(eSuccess == newObject.first)
				{
					result.second.push_back(newObject.second);
				}
				else
				{
					TRACE_LOG1("PDFDocumentHandler::AppendPDFPagesFromPDF, failed to embed page %ld", i);
					result.first = eFailure;
				}
			}
		}
		else
		{
			// eRangeTypeSpecific
			ULongAndULongList::const_iterator it = inPageRange.mSpecificRanges.begin();
			for(; it != inPageRange.mSpecificRanges.end() && eSuccess == result.first;++it)
			{
				if(it->first <= it->second && it->second < mParser.GetPagesCount())
				{
					for(unsigned long i=it->first; i <= it->second && eSuccess == result.first; ++i)
					{
						newObject = CreatePDFPageForPage(i);
						if(eSuccess == newObject.first)
						{
							result.second.push_back(newObject.second);
						}
						else
						{
							TRACE_LOG1("PDFDocumentHandler::AppendPDFPagesFromPDF, failed to embed page %ld", i);
							result.first = eFailure;
						}
					}
				}
				else
				{
					TRACE_LOG3("PDFDocumentHandler::AppendPDFPagesFromPDF, range mismatch. first = %ld, second = %ld, PDF page count = %ld", 
						it->first,
						it->second,
						mParser.GetPagesCount());
					result.first = eFailure;
				}
			}
		}


	}while(false);

	IDocumentContextExtenderSet::iterator it = mExtenders.begin();
	for(; it != mExtenders.end() && eSuccess == result.first; ++it)
	{
		result.first = (*it)->OnPDFCopyingComplete(mObjectsContext,mDocumentContext,this);
		if(result.first != eSuccess)
			TRACE_LOG("DocumentContext::AppendPDFPagesFromPDF, unexpected failure. extender declared failure before finalizing copy.");
	}

	StopFileCopyingContext();

	return result;
}

EStatusCodeAndObjectIDType PDFDocumentHandler::CreatePDFPageForPage(unsigned long inPageIndex)
{
	RefCountPtr<PDFDictionary> pageObject = mParser.ParsePage(inPageIndex);
	EStatusCodeAndObjectIDType result;
	result.first = eFailure;
	result.second = 0;
	PDFPage* newPage = NULL;

	if(!pageObject)
	{
		TRACE_LOG1("PDFDocumentHandler::CreatePDFPageForPage, unhexpected exception, page index does not denote a page object. page index = %ld",inPageIndex);
		return result;
	}

	EStatusCode status = eSuccess;

	IDocumentContextExtenderSet::iterator it = mExtenders.begin();
	for(; it != mExtenders.end() && eSuccess == status; ++it)
	{
		result.first = (*it)->OnBeforeCreatePageFromPage(pageObject.GetPtr(),mObjectsContext,mDocumentContext,this);
		if(result.first != eSuccess)
			TRACE_LOG("DocumentContext::CreatePDFPageForPage, unexpected failure. extender declared failure before writing page.");
	}
	if(status != eSuccess)
		return result;

	do
	{
		if(CopyResourcesIndirectObjects(pageObject.GetPtr()) != eSuccess)
			break;

		// Create a new form XObject
		newPage = new PDFPage();
		newPage->SetMediaBox(DeterminePageBox(pageObject.GetPtr(),ePDFPageBoxMediaBox));

		// copy the page content to the target page content
		if(CopyPageContentToTargetPage(newPage,pageObject.GetPtr()) != eSuccess)
		{
			delete newPage;
			newPage = NULL;
			break;
		}

		// resources dictionary is gonna be empty at this point...so we can use our own code to write the dictionary, by extending.
		// which will be a simple loop. note that at this point all indirect objects should have been copied, and have mapping
		mDocumentContext->AddDocumentContextExtender(this);
		mWrittenPage = pageObject.GetPtr();

		result = mDocumentContext->WritePage(newPage);
		if(result.first != eSuccess)
		{
			delete newPage;
			newPage = NULL;
			break;
		}

	}while(false);

	mWrittenPage = NULL;
	mDocumentContext->RemoveDocumentContextExtender(this);

	if(result.first = eSuccess)
	{
		it = mExtenders.begin();
		for(; it != mExtenders.end() && eSuccess == result.first; ++it)
		{
			result.first = (*it)->OnAfterCreatePageFromPage(newPage,pageObject.GetPtr(),mObjectsContext,mDocumentContext,this);
			if(result.first != eSuccess)
				TRACE_LOG("DocumentContext::CreatePDFFormXObjectForPage, unexpected failure. extender declared failure after writing page.");
		}
	}

	delete newPage;

	return result;	
}

EStatusCode PDFDocumentHandler::CopyPageContentToTargetPage(PDFPage* inPage,PDFDictionary* inPageObject)
{
	EStatusCode status = eSuccess;

	PageContentContext* pageContentContext = mDocumentContext->StartPageContentContext(inPage);

	RefCountPtr<PDFObject> pageContent(mParser.QueryDictionaryObject(inPageObject,"Contents"));
	if(pageContent->GetType() == ePDFObjectStream)
	{
		status = WritePDFStreamInputToContentContext(pageContentContext,(PDFStreamInput*)pageContent.GetPtr());
	}
	else if(pageContent->GetType() == ePDFObjectArray)
	{
		SingleValueContainerIterator<PDFObjectVector> it = ((PDFArray*)pageContent.GetPtr())->GetIterator();
		PDFObjectCastPtr<PDFIndirectObjectReference> refItem;
		while(it.MoveNext() && status == eSuccess)
		{
			refItem = it.GetItem();
			if(!refItem)
			{
				status = eFailure;
				TRACE_LOG("PDFDocumentHandler::CopyPageContentToTargetPage, content stream array contains non-refs");
				break;
			}
			PDFObjectCastPtr<PDFStreamInput> contentStream(mParser.ParseNewObject(refItem->mObjectID));
			if(!contentStream)
			{
				status = eFailure;
				TRACE_LOG("PDFDocumentHandler::CopyPageContentToTargetPage, content stream array contains references to non streams");
				break;
			}
			status = WritePDFStreamInputToContentContext(pageContentContext,contentStream.GetPtr());
		}
	}
	else
	{
		TRACE_LOG1("PDFDocumentHandler::CopyPageContentToTargetPage, error copying page content, expected either array or stream, getting %s",scPDFObjectTypeLabel[pageContent->GetType()]);
		status = eFailure;
	}
	
	if(status != eSuccess)
	{
		delete pageContentContext;
	}
	else
	{
		mDocumentContext->EndPageContentContext(pageContentContext);
	}
	return status;
}

EStatusCode PDFDocumentHandler::WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource)
{
	EStatusCode status = eSuccess;
	
	do
	{
		inContentContext->StartAStreamIfRequired();

		status = WritePDFStreamInputToStream(inContentContext->GetCurrentPageContentStream()->GetWriteStream(),inContentSource);
		if(status != eSuccess)
		{
			TRACE_LOG("PDFDocumentHandler::WritePDFStreamInputToContentContext, failed to write content stream from page input to target page");
			break;
		}

		status = inContentContext->FinalizeCurrentStream();

	}while(false);

	return status;

}

static const string scParent = "Parent";
PDFObject* PDFDocumentHandler::QueryInheritedValue(PDFDictionary* inDictionary,string inName)
{
	if(inDictionary->Exists(inName))
	{
		return mParser.QueryDictionaryObject(inDictionary,inName);
	}
	else if(inDictionary->Exists(scParent))
	{
		PDFObjectCastPtr<PDFDictionary> parent(mParser.QueryDictionaryObject(inDictionary,scParent));
		if(!parent)
			return NULL;
		return QueryInheritedValue(parent.GetPtr(),inName);
	}
	else
		return NULL;
}

EStatusCode PDFDocumentHandler::StartFileCopyingContext(const wstring& inPDFFilePath)
{
	EStatusCode status;

	do
	{
		status = mPDFFile.OpenFile(inPDFFilePath);
		if(status != eSuccess)
		{
			TRACE_LOG1("PDFDocumentHandler::StartFileCopyingContext, unable to open file for reading in %s",inPDFFilePath.c_str());
			break;
		}

		status = mParser.StartPDFParsing(mPDFFile.GetInputStream());
		if(status != eSuccess)
		{
			TRACE_LOG("PDFDocumentHandler::StartFileCopyingContext, failure occured while parsing PDF file.");
			break;
		}

		// do not allow encrypted/protected documents
		PDFObjectCastPtr<PDFDictionary> encryptionDictionary(mParser.QueryDictionaryObject(mParser.GetTrailer(),"Encrypt"));
		if(encryptionDictionary.GetPtr())
		{
			TRACE_LOG("PDFDocumentHandler::StartFileCopyingContext, Document contains an encryption dictionary. Library does not support embedding of encrypted PDF");
			status = eFailure;
			break;
		}

	}while(false);

	return status;
}

EStatusCodeAndObjectIDType PDFDocumentHandler::CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
																			EPDFPageBox inPageBoxToUseAsFormBox,
																			const double* inTransformationMatrix)
{
	EStatusCodeAndObjectIDType result;
	PDFFormXObject* newObject;

	if(inPageIndex < mParser.GetPagesCount())
	{
		newObject = CreatePDFFormXObjectForPage(inPageIndex,inPageBoxToUseAsFormBox,inTransformationMatrix);
		if(newObject)
		{
			result.first = eSuccess;
			result.second = newObject->GetObjectID();
			delete newObject;
		}
		else
		{
			TRACE_LOG1("PDFDocumentHandler::CreateFormXObjectFromPDFPage, failed to embed page %ld",inPageIndex);
			result.first = eFailure;
		}
	}
	else
	{
		TRACE_LOG2(
			"PDFDocumentHandler::CreateFormXObjectFromPDFPage, request object index %ld is larger than maximum page for input document = %ld", 
			inPageIndex,
			mParser.GetPagesCount()-1);
		result.first = eFailure;
	}
	return result;
}

EStatusCodeAndObjectIDType PDFDocumentHandler::CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
																			 const PDFRectangle& inCropBox,
																			 const double* inTransformationMatrix)
{
	EStatusCodeAndObjectIDType result;
	PDFFormXObject* newObject;

	if(inPageIndex < mParser.GetPagesCount())
	{
		newObject = CreatePDFFormXObjectForPage(inPageIndex,inCropBox,inTransformationMatrix);
		if(newObject)
		{
			result.first = eSuccess;
			result.second = newObject->GetObjectID();
			delete newObject;
		}
		else
		{
			TRACE_LOG1("PDFDocumentHandler::CreateFormXObjectFromPDFPage, failed to embed page %ld",inPageIndex);
			result.first = eFailure;
		}
	}
	else
	{
		TRACE_LOG2(
			"PDFDocumentHandler::CreateFormXObjectFromPDFPage, request object index %ld is larger than maximum page for input document = %ld", 
			inPageIndex,
			mParser.GetPagesCount()-1);
		result.first = eFailure;
	}
	return result;
}


EStatusCodeAndObjectIDType PDFDocumentHandler::AppendPDFPageFromPDF(unsigned long inPageIndex)
{
	EStatusCodeAndObjectIDType result;

	if(inPageIndex < mParser.GetPagesCount())
	{
		result = CreatePDFPageForPage(inPageIndex);
		if(result.first != eSuccess)
			TRACE_LOG1("PDFDocumentHandler::AppendPDFPageFromPDF, failed to append page %ld",inPageIndex);
	}
	else
	{
		TRACE_LOG2(
			"PDFDocumentHandler::AppendPDFPageFromPDF, request object index %ld is larger than maximum page for input document = %ld", 
			inPageIndex,
			mParser.GetPagesCount()-1);
		result.first = eFailure;
	}
	return result;
}

EStatusCodeAndObjectIDType PDFDocumentHandler::CopyObject(ObjectIDType inSourceObjectID)
{
	EStatusCodeAndObjectIDType result;

	ObjectIDTypeToObjectIDTypeMap::iterator it = mSourceToTarget.find(inSourceObjectID);
	if(it == mSourceToTarget.end())
	{
		ObjectIDTypeList anObjectList;
		anObjectList.push_back(inSourceObjectID);
		result.first = WriteNewObjects(anObjectList);
		result.second = mSourceToTarget[inSourceObjectID];
	}
	else
	{
		result.first = eSuccess;
		result.second = it->second;
	}
	return result;
}

PDFParser* PDFDocumentHandler::GetSourceDocumentParser()
{
	return &mParser;
}

EStatusCodeAndObjectIDType PDFDocumentHandler::GetCopiedObjectID(ObjectIDType inSourceObjectID)
{
	EStatusCodeAndObjectIDType result;

	ObjectIDTypeToObjectIDTypeMap::iterator it = mSourceToTarget.find(inSourceObjectID);
	if(it == mSourceToTarget.end())
	{
		result.first = eFailure;
	}
	else
	{
		result.first = eSuccess;
		result.second = it->second;
	}
	return result;
}

MapIterator<ObjectIDTypeToObjectIDTypeMap> PDFDocumentHandler::GetCopiedObjectsMappingIterator()
{
	return MapIterator<ObjectIDTypeToObjectIDTypeMap>(mSourceToTarget);
}

void PDFDocumentHandler::StopFileCopyingContext()
{
	mPDFFile.CloseFile();
	// clearing the source to target mapping here. note that copying enjoyed sharing of objects between them
	mSourceToTarget.clear();
	mParser.ResetParser();

}

void PDFDocumentHandler::AddDocumentContextExtender(IDocumentContextExtender* inExtender)
{
	mExtenders.insert(inExtender);
}

void PDFDocumentHandler::RemoveDocumentContextExtender(IDocumentContextExtender* inExtender)
{
	mExtenders.erase(inExtender);
}
