/*
   Source File : DocumentContext.cpp


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
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "IByteWriterWithPosition.h"
#include "DictionaryContext.h"
#include "PDFPage.h"
#include "PageTree.h"
#include "BoxingBase.h"
#include "InfoDictionary.h"
#include "MD5Generator.h"
#include "OutputFile.h"
#include "Trace.h"
#include "IDocumentContextExtender.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFIndirectObjectReference.h"
#include "PDFInteger.h"
#include "PDFLiteralString.h"
#include "PDFBoolean.h"
#include "PDFArray.h"
#include "PDFDocumentCopyingContext.h"
#include "Ascii7Encoding.h"

DocumentContext::DocumentContext()
{
	mObjectsContext = NULL;
}

DocumentContext::~DocumentContext(void)
{
}

void DocumentContext::SetObjectsContext(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
	mJPEGImageHandler.SetOperationsContexts(this,mObjectsContext);
	mTIFFImageHandler.SetOperationsContexts(this,mObjectsContext);
	mPDFDocumentHandler.SetOperationsContexts(this,mObjectsContext);
	mUsedFontsRepository.SetObjectsContext(mObjectsContext);
}

void DocumentContext::SetOutputFileInformation(OutputFile* inOutputFile)
{
	// just save the output file path for the ID generation in the end
	mOutputFilePath = inOutputFile->GetFilePath();
}

void DocumentContext::AddDocumentContextExtender(IDocumentContextExtender* inExtender)
{
	mExtenders.insert(inExtender);
	mJPEGImageHandler.AddDocumentContextExtender(inExtender);
	mPDFDocumentHandler.AddDocumentContextExtender(inExtender);
}

void DocumentContext::RemoveDocumentContextExtender(IDocumentContextExtender* inExtender)
{
	mExtenders.erase(inExtender);
	mJPEGImageHandler.RemoveDocumentContextExtender(inExtender);
	mPDFDocumentHandler.RemoveDocumentContextExtender(inExtender);
}

TrailerInformation& DocumentContext::GetTrailerInformation()
{
	return mTrailerInformation;
}

EStatusCode	DocumentContext::WriteHeader(EPDFVersion inPDFVersion)
{
	if(mObjectsContext)
	{
		WriteHeaderComment(inPDFVersion);
		Write4BinaryBytes();
		return eSuccess;
	}
	else
		return eFailure;
}

static const string scPDFVersion10 = "PDF-1.0";
static const string scPDFVersion11 = "PDF-1.1";
static const string scPDFVersion12 = "PDF-1.2";
static const string scPDFVersion13 = "PDF-1.3";
static const string scPDFVersion14 = "PDF-1.4";
static const string scPDFVersion15 = "PDF-1.5";
static const string scPDFVersion16 = "PDF-1.6";
static const string scPDFVersion17 = "PDF-1.7";

void DocumentContext::WriteHeaderComment(EPDFVersion inPDFVersion)
{
	switch(inPDFVersion)
	{
		case ePDFVersion10:
			mObjectsContext->WriteComment(scPDFVersion10);
			break;
		case ePDFVersion11:
			mObjectsContext->WriteComment(scPDFVersion11);
			break;
		case ePDFVersion12:
			mObjectsContext->WriteComment(scPDFVersion12);
			break;
		case ePDFVersion13:
			mObjectsContext->WriteComment(scPDFVersion13);
			break;
		case ePDFVersion14:
			mObjectsContext->WriteComment(scPDFVersion14);
			break;
		case ePDFVersion15:
			mObjectsContext->WriteComment(scPDFVersion15);
			break;
		case ePDFVersion16:
			mObjectsContext->WriteComment(scPDFVersion16);
			break;
		case ePDFVersion17:
			mObjectsContext->WriteComment(scPDFVersion17);
			break;
	}
}

static const IOBasicTypes::Byte scBinaryBytesArray[] = {'%',0xBD,0xBE,0xBC,'\r','\n'}; // might imply that i need a newline writer here....an underlying primitives-token context

void DocumentContext::Write4BinaryBytes()
{
	IByteWriterWithPosition *freeContextOutput = mObjectsContext->StartFreeContext();
	freeContextOutput->Write(scBinaryBytesArray,6);
	mObjectsContext->EndFreeContext();
}

EStatusCode	DocumentContext::FinalizePDF()
{
	EStatusCode status;
	LongFilePositionType xrefTablePosition;


	// this will finalize writing all renments of the file, like xref, trailer and whatever objects still accumulating
	do
	{
		status = WriteUsedFontsDefinitions();
		if(status != 0)
			break;

		WritePagesTree();

		status = WriteCatalogObject();
		if(status != 0)
			break;


		// write the info dictionary of the trailer, if has any valid entries
		WriteInfoDictionary();

		status = mObjectsContext->WriteXrefTable(xrefTablePosition);
		if(status != 0)
			break;

		status = WriteTrailerDictionary();
		if(status != 0)
			break;

		WriteXrefReference(xrefTablePosition);
		WriteFinalEOF();
		
	} while(false);

	return status;
}


static const string scStartXref = "startxref";
void DocumentContext::WriteXrefReference(LongFilePositionType inXrefTablePosition)
{
	mObjectsContext->WriteKeyword(scStartXref);
	mObjectsContext->WriteInteger(inXrefTablePosition,eTokenSeparatorEndLine);
}

static const IOBasicTypes::Byte scEOF[] = {'%','%','E','O','F'}; 

void DocumentContext::WriteFinalEOF()
{
	IByteWriterWithPosition *freeContextOutput = mObjectsContext->StartFreeContext();
	freeContextOutput->Write(scEOF,5);
	mObjectsContext->EndFreeContext();
}

static const string scTrailer = "trailer";
static const string scSize = "Size";
static const string scPrev = "Prev";
static const string scRoot = "Root";
static const string scEncrypt = "Encrypt";
static const string scInfo = "Info";
static const string scID = "ID";
EStatusCode DocumentContext::WriteTrailerDictionary()
{
	EStatusCode status = eSuccess;
	DictionaryContext* dictionaryContext;

	mObjectsContext->WriteKeyword(scTrailer);
	dictionaryContext = mObjectsContext->StartDictionary();

	do
	{
	
		// size
		dictionaryContext->WriteKey(scSize);
		dictionaryContext->WriteIntegerValue(mObjectsContext->GetInDirectObjectsRegistry().GetObjectsCount());

		// prev reference
		BoolAndLongFilePositionType filePositionResult = mTrailerInformation.GetPrev(); 
		if(filePositionResult.first)
		{
			dictionaryContext->WriteKey(scPrev);
			dictionaryContext->WriteIntegerValue(filePositionResult.second);
		}

		// catalog reference
		BoolAndObjectIDType objectIDResult = mTrailerInformation.GetRoot();
		if(objectIDResult.first)
		{
			dictionaryContext->WriteKey(scRoot);
			dictionaryContext->WriteObjectReferenceValue(objectIDResult.second);
		}
		else
		{
			TRACE_LOG("DocumentContext::WriteTrailerDictionary, Unexpected Failure. Didn't find catalog object while writing trailer");
			status = eFailure;
			break;
		}

		// encrypt dictionary reference
		objectIDResult = mTrailerInformation.GetEncrypt();
		if(objectIDResult.first)
		{
			dictionaryContext->WriteKey(scEncrypt);
			dictionaryContext->WriteObjectReferenceValue(objectIDResult.second);
		}

		// info reference
		objectIDResult = mTrailerInformation.GetInfoDictionaryReference();
		if(objectIDResult.first)
		{
			dictionaryContext->WriteKey(scInfo);
			dictionaryContext->WriteObjectReferenceValue(objectIDResult.second);
		}

		// write ID

		string id = GenerateMD5IDForFile();
		dictionaryContext->WriteKey(scID);
		mObjectsContext->StartArray();
		mObjectsContext->WriteHexString(id);
		mObjectsContext->WriteHexString(id);
		mObjectsContext->EndArray();
		mObjectsContext->EndLine();

	}while(false);
	
	mObjectsContext->EndDictionary(dictionaryContext);

	return status;
}

static const string scTitle = "Title";
static const string scAuthor = "Author";
static const string scSubject = "Subject";
static const string scKeywords = "Keywords";
static const string scCreator = "Creator";
static const string scProducer = "Producer";
static const string scCreationDate = "CreationDate";
static const string scModDate = "ModDate";
static const string scTrapped = "Trapped";
static const string scTrue = "True";
static const string scFalse = "False";

void DocumentContext::WriteInfoDictionary()
{
	InfoDictionary& infoDictionary = mTrailerInformation.GetInfo();
	if(infoDictionary.IsEmpty())
		return;

	ObjectIDType infoDictionaryID = mObjectsContext->StartNewIndirectObject();
	DictionaryContext* infoContext = mObjectsContext->StartDictionary();

	mTrailerInformation.SetInfoDictionaryReference(infoDictionaryID);

	if(!infoDictionary.Title.IsEmpty()) 
	{
		infoContext->WriteKey(scTitle);
		infoContext->WriteLiteralStringValue(infoDictionary.Title.ToString());
	}
	
	if(!infoDictionary.Author.IsEmpty()) 
	{
		infoContext->WriteKey(scAuthor);
		infoContext->WriteLiteralStringValue(infoDictionary.Author.ToString());
	}

	if(!infoDictionary.Subject.IsEmpty()) 
	{
		infoContext->WriteKey(scSubject);
		infoContext->WriteLiteralStringValue(infoDictionary.Subject.ToString());
	}

	if(!infoDictionary.Keywords.IsEmpty()) 
	{
		infoContext->WriteKey(scKeywords);
		infoContext->WriteLiteralStringValue(infoDictionary.Keywords.ToString());
	}

	if(!infoDictionary.Creator.IsEmpty()) 
	{
		infoContext->WriteKey(scCreator);
		infoContext->WriteLiteralStringValue(infoDictionary.Creator.ToString());
	}

	if(!infoDictionary.Producer.IsEmpty()) 
	{
		infoContext->WriteKey(scProducer);
		infoContext->WriteLiteralStringValue(infoDictionary.Producer.ToString());
	}

	if(!infoDictionary.CreationDate.IsNull()) 
	{
		infoContext->WriteKey(scCreationDate);
		infoContext->WriteLiteralStringValue(infoDictionary.CreationDate.ToString());
	}

	if(!infoDictionary.ModDate.IsNull()) 
	{
		infoContext->WriteKey(scModDate);
		infoContext->WriteLiteralStringValue(infoDictionary.ModDate.ToString());
	}

	if(EInfoTrappedUnknown != infoDictionary.Trapped)
	{
		infoContext->WriteKey(scTrapped);
		infoContext->WriteNameValue(EInfoTrappedTrue == infoDictionary.Trapped ? scTrue : scFalse);
	}

	MapIterator<StringToPDFTextString> it = infoDictionary.GetAdditionaEntriesIterator();

	while(it.MoveNext())
	{
		infoContext->WriteKey(it.GetKey());
		infoContext->WriteLiteralStringValue(it.GetValue().ToString());
	}

	mObjectsContext->EndDictionary(infoContext);
	mObjectsContext->EndIndirectObject();
	
}

CatalogInformation& DocumentContext::GetCatalogInformation()
{
	return mCatalogInformation;
}

static const string scType = "Type";
static const string scCatalog = "Catalog";
static const string scPages = "Pages";
EStatusCode DocumentContext::WriteCatalogObject()
{
	EStatusCode status = eSuccess;
	ObjectIDType catalogID = mObjectsContext->StartNewIndirectObject();
	mTrailerInformation.SetRoot(catalogID); // set the catalog reference as root in the trailer

	DictionaryContext* catalogContext = mObjectsContext->StartDictionary();

	catalogContext->WriteKey(scType);
	catalogContext->WriteNameValue(scCatalog);

	catalogContext->WriteKey(scPages);
	catalogContext->WriteObjectReferenceValue(mCatalogInformation.GetPageTreeRoot(mObjectsContext->GetInDirectObjectsRegistry())->GetID());

	IDocumentContextExtenderSet::iterator it = mExtenders.begin();
	for(; it != mExtenders.end() && eSuccess == status; ++it)
	{
		status = (*it)->OnCatalogWrite(&mCatalogInformation,catalogContext,mObjectsContext,this);
		if(status != eSuccess)
			TRACE_LOG("DocumentContext::WriteCatalogObject, unexpected failure. extender declared failure when writing catalog.");
	}

	mObjectsContext->EndDictionary(catalogContext);
	mObjectsContext->EndIndirectObject();
	return status;
}


void DocumentContext::WritePagesTree()
{
	PageTree* pageTreeRoot = mCatalogInformation.GetPageTreeRoot(mObjectsContext->GetInDirectObjectsRegistry());

	WritePageTree(pageTreeRoot);
}

static const string scCount = "Count";
static const string scKids = "Kids";
static const string scParent = "Parent";

// Recursion to write a page tree node. the return result is the page nodes count, for
// accumulation at higher levels
int DocumentContext::WritePageTree(PageTree* inPageTreeToWrite)
{
	DictionaryContext* pagesTreeContext;

	if(inPageTreeToWrite->IsLeafParent())
	{
		mObjectsContext->StartNewIndirectObject(inPageTreeToWrite->GetID());

		pagesTreeContext = mObjectsContext->StartDictionary();

		// type
		pagesTreeContext->WriteKey(scType);
		pagesTreeContext->WriteNameValue(scPages);
		
		// count
		pagesTreeContext->WriteKey(scCount);
		pagesTreeContext->WriteIntegerValue(inPageTreeToWrite->GetNodesCount());

		// kids
		pagesTreeContext->WriteKey(scKids);
		mObjectsContext->StartArray();
		for(int i=0;i<inPageTreeToWrite->GetNodesCount();++i)
			mObjectsContext->WriteIndirectObjectReference(inPageTreeToWrite->GetPageIDChild(i));
		mObjectsContext->EndArray();
		mObjectsContext->EndLine();

		// parent
		if(inPageTreeToWrite->GetParent())
		{
			pagesTreeContext->WriteKey(scParent);
			pagesTreeContext->WriteObjectReferenceValue(inPageTreeToWrite->GetParent()->GetID());
		}

		mObjectsContext->EndDictionary(pagesTreeContext);
		mObjectsContext->EndIndirectObject();

		return inPageTreeToWrite->GetNodesCount();
	}
	else
	{
		int totalPagesNodes = 0;

		// first loop the kids and write them (while at it, accumulate the children count).
		for(int i=0;i<inPageTreeToWrite->GetNodesCount();++i)
			totalPagesNodes += WritePageTree(inPageTreeToWrite->GetPageTreeChild(i));

		mObjectsContext->StartNewIndirectObject(inPageTreeToWrite->GetID());

		pagesTreeContext = mObjectsContext->StartDictionary();

		// type
		pagesTreeContext->WriteKey(scType);
		pagesTreeContext->WriteNameValue(scPages);
		
		// count
		pagesTreeContext->WriteKey(scCount);
		pagesTreeContext->WriteIntegerValue(totalPagesNodes);

		// kids
		pagesTreeContext->WriteKey(scKids);
		mObjectsContext->StartArray();
		for(int j=0;j<inPageTreeToWrite->GetNodesCount();++j)
			mObjectsContext->WriteIndirectObjectReference(inPageTreeToWrite->GetPageTreeChild(j)->GetID());
		mObjectsContext->EndArray();
		mObjectsContext->EndLine();

		// parent
		if(inPageTreeToWrite->GetParent())
		{
			pagesTreeContext->WriteKey(scParent);
			pagesTreeContext->WriteObjectReferenceValue(inPageTreeToWrite->GetParent()->GetID());
		}

		mObjectsContext->EndDictionary(pagesTreeContext);
		mObjectsContext->EndIndirectObject();

		return totalPagesNodes;
	}
}

static const string scResources = "Resources";
static const string scPage = "Page";
static const string scMediaBox = "MediaBox";
static const string scContents = "Contents";

EStatusCodeAndObjectIDType DocumentContext::WritePage(PDFPage* inPage)
{
	EStatusCodeAndObjectIDType result;
	
	result.first = eSuccess;
	result.second = mObjectsContext->StartNewIndirectObject();

	DictionaryContext* pageContext = mObjectsContext->StartDictionary();

	// type
	pageContext->WriteKey(scType);
	pageContext->WriteNameValue(scPage);

	// parent
	pageContext->WriteKey(scParent);
	pageContext->WriteObjectReferenceValue(mCatalogInformation.AddPageToPageTree(result.second,mObjectsContext->GetInDirectObjectsRegistry()));
	
	// Media Box
	pageContext->WriteKey(scMediaBox);
	pageContext->WriteRectangleValue(inPage->GetMediaBox());

	do
	{
		// Resource dict 
		pageContext->WriteKey(scResources);
		result.first = WriteResourcesDictionary(inPage->GetResourcesDictionary());
		if(result.first != eSuccess)
		{
			TRACE_LOG("DocumentContext::WritePage, failed to write resources dictionary");
			break;
		}

		// Annotations
		if(mAnnotations.size() > 0)
		{
			pageContext->WriteKey("Annots");

			ObjectIDTypeList::iterator it = mAnnotations.begin();

			mObjectsContext->StartArray();
			for(; it != mAnnotations.end(); ++it)
				mObjectsContext->WriteIndirectObjectReference(*it);
			mObjectsContext->EndArray(eTokenSeparatorEndLine);			
		}
		mAnnotations.clear();

		// Content
		if(inPage->GetContentStreamsCount() > 0)
		{
			SingleValueContainerIterator<ObjectIDTypeList> iterator = inPage->GetContentStreamReferencesIterator();

			pageContext->WriteKey(scContents);
			if(inPage->GetContentStreamsCount() > 1)
			{
				mObjectsContext->StartArray();
				while(iterator.MoveNext())
					mObjectsContext->WriteIndirectObjectReference(iterator.GetItem());
				mObjectsContext->EndArray();	
				mObjectsContext->EndLine();
			}
			else
			{
				iterator.MoveNext();
				pageContext->WriteObjectReferenceValue(iterator.GetItem());
			}
		}

		IDocumentContextExtenderSet::iterator it = mExtenders.begin();
		for(; it != mExtenders.end() && eSuccess == result.first; ++it)
		{
			result.first = (*it)->OnPageWrite(inPage,pageContext,mObjectsContext,this);
			if(result.first != eSuccess)
			{
				TRACE_LOG("DocumentContext::WritePage, unexpected failure. extender declared failure when writing page.");
				break;
			}
		}
		result.first = mObjectsContext->EndDictionary(pageContext);
		if(result.first != eSuccess)
		{
			TRACE_LOG("DocumentContext::WritePage, unexpected failure. Failed to end dictionary in page write.");
			break;
		}
		mObjectsContext->EndIndirectObject();	
	}while(false);

	return result;
}


EStatusCodeAndObjectIDType DocumentContext::WritePageAndRelease(PDFPage* inPage)
{
	EStatusCodeAndObjectIDType status = WritePage(inPage);
	delete inPage;
	return status;
}

static const string scUnknown = "Unknown";
string DocumentContext::GenerateMD5IDForFile()
{
	MD5Generator md5;

	// encode current time
	PDFDate currentTime;
	currentTime.SetToCurrentTime();
	md5.Accumulate(currentTime.ToString());

	// file location
	md5.Accumulate(mOutputFilePath);

	// current writing position (will serve as "file size")
	IByteWriterWithPosition *outputStream = mObjectsContext->StartFreeContext();
	mObjectsContext->EndFreeContext();

	md5.Accumulate(BoxingBaseWithRW<LongFilePositionType>(outputStream->GetCurrentPosition()).ToString());

	// document information dictionary
	InfoDictionary& infoDictionary = mTrailerInformation.GetInfo();

	md5.Accumulate(infoDictionary.Title.ToString());
	md5.Accumulate(infoDictionary.Author.ToString());
	md5.Accumulate(infoDictionary.Subject.ToString());
	md5.Accumulate(infoDictionary.Keywords.ToString());
	md5.Accumulate(infoDictionary.Creator.ToString());
	md5.Accumulate(infoDictionary.Producer.ToString());
	md5.Accumulate(infoDictionary.CreationDate.ToString());
	md5.Accumulate(infoDictionary.ModDate.ToString());
	md5.Accumulate(EInfoTrappedUnknown == infoDictionary.Trapped ? scUnknown:(EInfoTrappedTrue == infoDictionary.Trapped ? scTrue:scFalse));

	MapIterator<StringToPDFTextString> it = infoDictionary.GetAdditionaEntriesIterator();

	while(it.MoveNext())
		md5.Accumulate(it.GetValue().ToString());

	return md5.ToString();
}

bool DocumentContext::HasContentContext(PDFPage* inPage)
{
	return inPage->GetAssociatedContentContext() != NULL;
}

PageContentContext* DocumentContext::StartPageContentContext(PDFPage* inPage)
{
	if(!inPage->GetAssociatedContentContext())
	{
		inPage->AssociateContentContext(new PageContentContext(inPage,mObjectsContext));
	}
	return inPage->GetAssociatedContentContext();
}

EStatusCode DocumentContext::PausePageContentContext(PageContentContext* inPageContext)
{
	return inPageContext->FinalizeCurrentStream();
}

EStatusCode DocumentContext::EndPageContentContext(PageContentContext* inPageContext)
{
	EStatusCode status = inPageContext->FinalizeCurrentStream();
	inPageContext->GetAssociatedPage()->DisassociateContentContext();
	delete inPageContext;
	return status;
}

static const string scXObject = "XObject";
static const string scSubType = "Subtype";
static const string scForm = "Form";
static const string scBBox = "BBox";
static const string scFormType = "FormType";
static const string scMatrix = "Matrix";
PDFFormXObject* DocumentContext::StartFormXObject(const PDFRectangle& inBoundingBox,ObjectIDType inFormXObjectID,const double* inMatrix)
{
	PDFFormXObject* aFormXObject = NULL;
	do
	{
		mObjectsContext->StartNewIndirectObject(inFormXObjectID);
		DictionaryContext* xobjectContext = mObjectsContext->StartDictionary();

		// type
		xobjectContext->WriteKey(scType);
		xobjectContext->WriteNameValue(scXObject);

		// subtype
		xobjectContext->WriteKey(scSubType);
		xobjectContext->WriteNameValue(scForm);

		// form type
		xobjectContext->WriteKey(scFormType);
		xobjectContext->WriteIntegerValue(1);

		// bbox
		xobjectContext->WriteKey(scBBox);
		xobjectContext->WriteRectangleValue(inBoundingBox);

		// matrix
		if(inMatrix && !IsIdentityMatrix(inMatrix))
		{
			xobjectContext->WriteKey(scMatrix);
			mObjectsContext->StartArray();
			for(int i=0;i<6;++i)
				mObjectsContext->WriteDouble(inMatrix[i]);
			mObjectsContext->EndArray(eTokenSeparatorEndLine);
		}

		// Resource dict 
		xobjectContext->WriteKey(scResources);	
		// put a resources dictionary place holder
		ObjectIDType formXObjectResourcesDictionaryID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
		xobjectContext->WriteObjectReferenceValue(formXObjectResourcesDictionaryID);

		IDocumentContextExtenderSet::iterator it = mExtenders.begin();
		EStatusCode status = eSuccess;
		for(; it != mExtenders.end() && eSuccess == status; ++it)
		{
			if((*it)->OnFormXObjectWrite(inFormXObjectID,formXObjectResourcesDictionaryID,xobjectContext,mObjectsContext,this) != eSuccess)
			{
				TRACE_LOG("DocumentContext::StartFormXObject, unexpected failure. extender declared failure when writing form xobject.");
				status = eFailure;
				break;
			}
		}
		if(status != eSuccess)
			break;

		// Now start the stream and the form XObject state
		aFormXObject =  new PDFFormXObject(inFormXObjectID,mObjectsContext->StartPDFStream(xobjectContext),formXObjectResourcesDictionaryID);
	} while(false);

	return aFormXObject;	
}


PDFFormXObject* DocumentContext::StartFormXObject(const PDFRectangle& inBoundingBox,const double* inMatrix)
{
	ObjectIDType formXObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
	return StartFormXObject(inBoundingBox,formXObjectID,inMatrix);
}

EStatusCode DocumentContext::EndFormXObjectNoRelease(PDFFormXObject* inFormXObject)
{
	mObjectsContext->EndPDFStream(inFormXObject->GetContentStream());

	// now write the resources dictionary, full of all the goodness that got accumulated over the stream write
	mObjectsContext->StartNewIndirectObject(inFormXObject->GetResourcesDictionaryObjectID());
	WriteResourcesDictionary(inFormXObject->GetResourcesDictionary());
	mObjectsContext->EndIndirectObject();
	
	return eSuccess;
}

EStatusCode DocumentContext::EndFormXObjectAndRelease(PDFFormXObject* inFormXObject)
{
	EStatusCode status = EndFormXObjectNoRelease(inFormXObject);
	delete inFormXObject; // will also delete the stream becuase the form XObject owns it
	
	return status;
}

static const string scXObjects = "XObject";
static const string scProcesets = "ProcSet";
static const string scExtGStates = "ExtGState";
static const string scFonts = "Font";
static const string scColorSpaces = "ColorSpace";
static const string scPatterns = "Pattern";
static const string scShadings = "Shading";
static const string scProperties = "Properties";
EStatusCode DocumentContext::WriteResourcesDictionary(ResourcesDictionary& inResourcesDictionary)
{
	EStatusCode status = eSuccess;

	do
	{

		DictionaryContext* resourcesContext = mObjectsContext->StartDictionary();

		if(inResourcesDictionary.GetProcsetsCount() > 0)
		{
			//	Procsets
			resourcesContext->WriteKey(scProcesets);
			mObjectsContext->StartArray();
			SingleValueContainerIterator<StringSet> it = inResourcesDictionary.GetProcesetsIterator();
			while(it.MoveNext())
				mObjectsContext->WriteName(it.GetItem());

			mObjectsContext->EndArray();
			mObjectsContext->EndLine();
		}

		if(inResourcesDictionary.GetXObjectsCount() >0)
		{
			// XObjects
			resourcesContext->WriteKey(scXObjects);
			DictionaryContext* xobjectsContext = mObjectsContext->StartDictionary();
			// form xobjects
			MapIterator<ObjectIDTypeToStringMap> itFormXObjects = inResourcesDictionary.GetFormXObjectsIterator();
			while(itFormXObjects.MoveNext())
			{
				xobjectsContext->WriteKey(itFormXObjects.GetValue());
				xobjectsContext->WriteObjectReferenceValue(itFormXObjects.GetKey());
			}
			// image xobjects
			MapIterator<ObjectIDTypeToStringMap> itImageXObjects = inResourcesDictionary.GetImageXObjectsIterator();
			while(itImageXObjects.MoveNext())
			{
				xobjectsContext->WriteKey(itImageXObjects.GetValue());
				xobjectsContext->WriteObjectReferenceValue(itImageXObjects.GetKey());
			}
			// generic xobjects
			MapIterator<ObjectIDTypeToStringMap> itGenericXObjects = inResourcesDictionary.GetGenericXObjectsIterator();
			while(itGenericXObjects.MoveNext())
			{
				xobjectsContext->WriteKey(itGenericXObjects.GetValue());
				xobjectsContext->WriteObjectReferenceValue(itGenericXObjects.GetKey());
			}
			mObjectsContext->EndDictionary(xobjectsContext);

			// i'm not having further extensiblity to XObjects. they will always be indirect, and user can use either of the three options to write xobject resources.
		}

		// ExtGStates
		if(inResourcesDictionary.GetExtGStatesCount() > 0)
			WriteResourceDictionary(resourcesContext,scExtGStates,inResourcesDictionary.GetExtGStatesIterator());

		// Fonts
		if(inResourcesDictionary.GetFontsCount() > 0)
			WriteResourceDictionary(resourcesContext,scFonts,inResourcesDictionary.GetFontsIterator());

		// Color space
		if(inResourcesDictionary.GetColorSpacesCount() > 0)
			WriteResourceDictionary(resourcesContext,scColorSpaces,inResourcesDictionary.GetColorSpacesIterator());
	
		// Patterns
		if(inResourcesDictionary.GetPatternsCount() > 0)
			WriteResourceDictionary(resourcesContext,scPatterns,inResourcesDictionary.GetPatternsIterator());

		// Shading
		if(inResourcesDictionary.GetShadingsCount() > 0)
			WriteResourceDictionary(resourcesContext,scShadings,inResourcesDictionary.GetShadingsIterator());

		// Properties
		if(inResourcesDictionary.GetPropertiesCount() > 0)
			WriteResourceDictionary(resourcesContext,scShadings,inResourcesDictionary.GetPropertiesIterator());

		IDocumentContextExtenderSet::iterator it = mExtenders.begin();
		for(; it != mExtenders.end() && eSuccess == status; ++it)
		{
			status = (*it)->OnResourcesWrite(&(inResourcesDictionary),resourcesContext,mObjectsContext,this);
			if(status != eSuccess)
			{
				TRACE_LOG("DocumentContext::WriteResourcesDictionary, unexpected failure. extender declared failure when writing resources.");
				break;
			}
		}

		mObjectsContext->EndDictionary(resourcesContext); 
	}while(false);

	return status;
}

void DocumentContext::WriteResourceDictionary(DictionaryContext* inResourcesDictionary,
											const string& inResourceDictionaryLabel,
											MapIterator<ObjectIDTypeToStringMap> inMapping)
{
	inResourcesDictionary->WriteKey(inResourceDictionaryLabel);
	DictionaryContext* resourceContext = mObjectsContext->StartDictionary();
	while(inMapping.MoveNext())
	{
		resourceContext->WriteKey(inMapping.GetValue());
		resourceContext->WriteObjectReferenceValue(inMapping.GetKey());
	}


	IDocumentContextExtenderSet::iterator it = mExtenders.begin();
	EStatusCode status = eSuccess;
	for(; it != mExtenders.end() && eSuccess == status; ++it)
	{
		status = (*it)->OnResourceDictionaryWrite(resourceContext,inResourceDictionaryLabel,mObjectsContext,this);
		if(status != eSuccess)
		{
			TRACE_LOG("DocumentContext::WriteResourceDictionary, unexpected failure. extender declared failure when writing a resource dictionary.");
			break;
		}
	}

	mObjectsContext->EndDictionary(resourceContext);	
}



bool DocumentContext::IsIdentityMatrix(const double* inMatrix)
{
	return 
		inMatrix[0] == 1 &&
		inMatrix[1] == 0 &&
		inMatrix[2] == 0 &&
		inMatrix[3] == 1 &&
		inMatrix[4] == 0 &&
		inMatrix[5] == 0;

}

PDFImageXObject* DocumentContext::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	return mJPEGImageHandler.CreateImageXObjectFromJPGFile(inJPGFilePath);
}

PDFFormXObject* DocumentContext::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	return mJPEGImageHandler.CreateFormXObjectFromJPGFile(inJPGFilePath);
}

JPEGImageHandler& DocumentContext::GetJPEGImageHandler()
{
	return mJPEGImageHandler;
}

PDFFormXObject* DocumentContext::CreateFormXObjectFromTIFFFile(	const wstring& inTIFFFilePath,
																const TIFFUsageParameters& inTIFFUsageParameters)
{
	
	return mTIFFImageHandler.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inTIFFUsageParameters);
}

PDFImageXObject* DocumentContext::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mJPEGImageHandler.CreateImageXObjectFromJPGFile(inJPGFilePath,inImageXObjectID);
}

PDFFormXObject* DocumentContext::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inFormXObjectID)
{
	return mJPEGImageHandler.CreateFormXObjectFromJPGFile(inJPGFilePath,inFormXObjectID);
}

PDFFormXObject* DocumentContext::CreateFormXObjectFromTIFFFile(	
												const wstring& inTIFFFilePath,
												ObjectIDType inFormXObjectID,
												const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mTIFFImageHandler.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inFormXObjectID,inTIFFUsageParameters);
}


PDFUsedFont* DocumentContext::GetFontForFile(const wstring& inFontFilePath)
{
	return mUsedFontsRepository.GetFontForFile(inFontFilePath);
}

EStatusCode DocumentContext::WriteUsedFontsDefinitions()
{
	return mUsedFontsRepository.WriteUsedFontsDefinitions();
}

PDFUsedFont* DocumentContext::GetFontForFile(const wstring& inFontFilePath,const wstring& inAdditionalMeticsFilePath)
{
	return mUsedFontsRepository.GetFontForFile(inFontFilePath,inAdditionalMeticsFilePath);
}

EStatusCodeAndObjectIDTypeList DocumentContext::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
																			const PDFPageRange& inPageRange,
																			EPDFPageBox inPageBoxToUseAsFormBox,
																			const double* inTransformationMatrix,
																			const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mPDFDocumentHandler.CreateFormXObjectsFromPDF(inPDFFilePath,inPageRange,inPageBoxToUseAsFormBox,inTransformationMatrix,inCopyAdditionalObjects);	

}

EStatusCodeAndObjectIDTypeList DocumentContext::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
																			const PDFPageRange& inPageRange,
																			const PDFRectangle& inCropBox,
																			const double* inTransformationMatrix,
																			const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mPDFDocumentHandler.CreateFormXObjectsFromPDF(inPDFFilePath,inPageRange,inCropBox,inTransformationMatrix,inCopyAdditionalObjects);	

}
EStatusCodeAndObjectIDTypeList DocumentContext::AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
																	  const PDFPageRange& inPageRange,
																	  const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mPDFDocumentHandler.AppendPDFPagesFromPDF(inPDFFilePath,inPageRange,inCopyAdditionalObjects);	
}

EStatusCode DocumentContext::WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	EStatusCode status;

	do
	{
		inStateWriter->StartNewIndirectObject(inObjectID);

		ObjectIDType trailerInformationID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
		ObjectIDType catalogInformationID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
		ObjectIDType usedFontsRepositoryID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();

		DictionaryContext* documentDictionary = inStateWriter->StartDictionary();

		documentDictionary->WriteKey("Type");
		documentDictionary->WriteNameValue("DocumentContext");

		documentDictionary->WriteKey("mTrailerInformation");
		documentDictionary->WriteObjectReferenceValue(trailerInformationID);

		documentDictionary->WriteKey("mCatalogInformation");
		documentDictionary->WriteObjectReferenceValue(catalogInformationID);

		documentDictionary->WriteKey("mUsedFontsRepository");
		documentDictionary->WriteObjectReferenceValue(usedFontsRepositoryID);

		inStateWriter->EndDictionary(documentDictionary);
		inStateWriter->EndIndirectObject();

		WriteTrailerState(inStateWriter,trailerInformationID);
		WriteCatalogInformationState(inStateWriter,catalogInformationID);
		
		status = mUsedFontsRepository.WriteState(inStateWriter,usedFontsRepositoryID);
		if(status != eSuccess)
			break;
	}while(false);

	return status;
}

void DocumentContext::WriteTrailerState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	inStateWriter->StartNewIndirectObject(inObjectID);

	DictionaryContext* trailerDictionary = inStateWriter->StartDictionary();

	trailerDictionary->WriteKey("Type");
	trailerDictionary->WriteNameValue("TrailerInformation");

	trailerDictionary->WriteKey("mPrev");
	trailerDictionary->WriteIntegerValue(mTrailerInformation.GetPrev().second);

	trailerDictionary->WriteKey("mRootReference");
	trailerDictionary->WriteIntegerValue(mTrailerInformation.GetRoot().second);

	trailerDictionary->WriteKey("mEncryptReference");
	trailerDictionary->WriteIntegerValue(mTrailerInformation.GetEncrypt().second);

	trailerDictionary->WriteKey("mInfoDictionary");
	ObjectIDType infoDictionaryID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
	trailerDictionary->WriteObjectReferenceValue(infoDictionaryID);

	trailerDictionary->WriteKey("mInfoDictionaryReference");
	trailerDictionary->WriteIntegerValue(mTrailerInformation.GetInfoDictionaryReference().second);

	inStateWriter->EndDictionary(trailerDictionary);
	inStateWriter->EndIndirectObject();

	WriteTrailerInfoState(inStateWriter,infoDictionaryID);
}

void DocumentContext::WriteTrailerInfoState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	inStateWriter->StartNewIndirectObject(inObjectID);
	DictionaryContext* infoDictionary = inStateWriter->StartDictionary();

	infoDictionary->WriteKey("Type");
	infoDictionary->WriteNameValue("InfoDictionary");

	infoDictionary->WriteKey("Title");
	infoDictionary->WriteLiteralStringValue(mTrailerInformation.GetInfo().Title.ToString());

	infoDictionary->WriteKey("Author");
	infoDictionary->WriteLiteralStringValue(mTrailerInformation.GetInfo().Author.ToString());

	infoDictionary->WriteKey("Subject");
	infoDictionary->WriteLiteralStringValue(mTrailerInformation.GetInfo().Subject.ToString());

	infoDictionary->WriteKey("Keywords");
	infoDictionary->WriteLiteralStringValue(mTrailerInformation.GetInfo().Keywords.ToString());

	infoDictionary->WriteKey("Creator");
	infoDictionary->WriteLiteralStringValue(mTrailerInformation.GetInfo().Creator.ToString());

	infoDictionary->WriteKey("Producer");
	infoDictionary->WriteLiteralStringValue(mTrailerInformation.GetInfo().Producer.ToString());

	infoDictionary->WriteKey("CreationDate");
	WriteDateState(inStateWriter,mTrailerInformation.GetInfo().CreationDate);

	infoDictionary->WriteKey("ModDate");
	WriteDateState(inStateWriter,mTrailerInformation.GetInfo().ModDate);

	infoDictionary->WriteKey("Trapped");
	infoDictionary->WriteIntegerValue(mTrailerInformation.GetInfo().Trapped);

	MapIterator<StringToPDFTextString> itAdditionalInfo = mTrailerInformation.GetInfo().GetAdditionaEntriesIterator();

	infoDictionary->WriteKey("mAdditionalInfoEntries");
	DictionaryContext* additionalInfoDictionary = inStateWriter->StartDictionary();
	while(itAdditionalInfo.MoveNext())
	{
		additionalInfoDictionary->WriteKey(itAdditionalInfo.GetKey());
		additionalInfoDictionary->WriteLiteralStringValue(itAdditionalInfo.GetValue().ToString());
	}
	inStateWriter->EndDictionary(additionalInfoDictionary);

	inStateWriter->EndDictionary(infoDictionary);
	inStateWriter->EndIndirectObject();

}

void DocumentContext::WriteDateState(ObjectsContext* inStateWriter,const PDFDate& inDate)
{
	DictionaryContext* dateDictionary = inStateWriter->StartDictionary();

	dateDictionary->WriteKey("Type");
	dateDictionary->WriteNameValue("Date");

	dateDictionary->WriteKey("Year");
	dateDictionary->WriteIntegerValue(inDate.Year);

	dateDictionary->WriteKey("Month");
	dateDictionary->WriteIntegerValue(inDate.Month);

	dateDictionary->WriteKey("Day");
	dateDictionary->WriteIntegerValue(inDate.Day);

	dateDictionary->WriteKey("Hour");
	dateDictionary->WriteIntegerValue(inDate.Hour);

	dateDictionary->WriteKey("Minute");
	dateDictionary->WriteIntegerValue(inDate.Minute);

	dateDictionary->WriteKey("Second");
	dateDictionary->WriteIntegerValue(inDate.Second);

	dateDictionary->WriteKey("UTC");
	dateDictionary->WriteIntegerValue(inDate.UTC);

	dateDictionary->WriteKey("HourFromUTC");
	dateDictionary->WriteIntegerValue(inDate.HourFromUTC);

	dateDictionary->WriteKey("MinuteFromUTC");
	dateDictionary->WriteIntegerValue(inDate.MinuteFromUTC);

	inStateWriter->EndDictionary(dateDictionary);
}

void DocumentContext::WriteCatalogInformationState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	ObjectIDType rootNodeID;
	if(mCatalogInformation.GetCurrentPageTreeNode())
	{
		rootNodeID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
		WritePageTreeState(inStateWriter,rootNodeID,mCatalogInformation.GetPageTreeRoot(mObjectsContext->GetInDirectObjectsRegistry()));
	}


	inStateWriter->StartNewIndirectObject(inObjectID);
	DictionaryContext* catalogInformation = inStateWriter->StartDictionary();

	catalogInformation->WriteKey("Type");
	catalogInformation->WriteNameValue("CatalogInformation");

	if(mCatalogInformation.GetCurrentPageTreeNode())
	{
		catalogInformation->WriteKey("PageTreeRoot");
		catalogInformation->WriteObjectReferenceValue(rootNodeID);

		catalogInformation->WriteKey("mCurrentPageTreeNode");
		catalogInformation->WriteObjectReferenceValue(mCurrentPageTreeIDInState);
	}

	inStateWriter->EndDictionary(catalogInformation);
	inStateWriter->EndIndirectObject();
	
}

void DocumentContext::WritePageTreeState(ObjectsContext* inStateWriter,ObjectIDType inObjectID,PageTree* inPageTree)
{
	ObjectIDTypeList kidsObjectIDs;

	inStateWriter->StartNewIndirectObject(inObjectID);
	DictionaryContext* pageTreeDictionary = inStateWriter->StartDictionary();
	
	pageTreeDictionary->WriteKey("Type");
	pageTreeDictionary->WriteNameValue("PageTree");

	pageTreeDictionary->WriteKey("mPageTreeID");
	pageTreeDictionary->WriteIntegerValue(inPageTree->GetID());

	pageTreeDictionary->WriteKey("mIsLeafParent");
	pageTreeDictionary->WriteBooleanValue(inPageTree->IsLeafParent());

	if(inPageTree->IsLeafParent())
	{
		pageTreeDictionary->WriteKey("mKidsIDs");
		inStateWriter->StartArray();
		for(int i=0;i<inPageTree->GetNodesCount();++i)
			inStateWriter->WriteInteger(inPageTree->GetPageIDChild(i));
		inStateWriter->EndArray(eTokenSeparatorEndLine);
	}
	else
	{
		pageTreeDictionary->WriteKey("mKidsNodes");
		inStateWriter->StartArray();
		for(int i=0;i<inPageTree->GetNodesCount();++i)
		{
			ObjectIDType pageNodeObjectID = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();
			inStateWriter->WriteIndirectObjectReference(pageNodeObjectID);
			kidsObjectIDs.push_back(pageNodeObjectID);
		}
		inStateWriter->EndArray(eTokenSeparatorEndLine);		
	}

	inStateWriter->EndDictionary(pageTreeDictionary);
	inStateWriter->EndIndirectObject();

	if(kidsObjectIDs.size() > 0)
	{
		ObjectIDTypeList::iterator it = kidsObjectIDs.begin();
		int i = 0;
		for(;i < inPageTree->GetNodesCount();++i,++it)
			WritePageTreeState(inStateWriter,*it,inPageTree->GetPageTreeChild(i));
	}

	if(inPageTree == mCatalogInformation.GetCurrentPageTreeNode())
	{
		mCurrentPageTreeIDInState = inObjectID;
	}
}

EStatusCode DocumentContext::ReadState(PDFParser* inStateReader,ObjectIDType inObjectID)
{
	PDFObjectCastPtr<PDFDictionary> documentState(inStateReader->ParseNewObject(inObjectID));

	PDFObjectCastPtr<PDFDictionary> trailerInformationState(inStateReader->QueryDictionaryObject(documentState.GetPtr(),"mTrailerInformation"));
	ReadTrailerState(inStateReader,trailerInformationState.GetPtr());

	PDFObjectCastPtr<PDFDictionary> catalogInformationState(inStateReader->QueryDictionaryObject(documentState.GetPtr(),"mCatalogInformation"));
	ReadCatalogInformationState(inStateReader,catalogInformationState.GetPtr());

	PDFObjectCastPtr<PDFIndirectObjectReference> usedFontsInformationStateID(documentState->QueryDirectObject("mUsedFontsRepository"));

	return mUsedFontsRepository.ReadState(inStateReader,usedFontsInformationStateID->mObjectID);
}

void DocumentContext::ReadTrailerState(PDFParser* inStateReader,PDFDictionary* inTrailerState)
{
	PDFObjectCastPtr<PDFInteger> prevState(inTrailerState->QueryDirectObject("mPrev"));
	mTrailerInformation.SetPrev(prevState->GetValue());

	PDFObjectCastPtr<PDFInteger> rootReferenceState(inTrailerState->QueryDirectObject("mRootReference"));
	mTrailerInformation.SetRoot((ObjectIDType)rootReferenceState->GetValue());

	PDFObjectCastPtr<PDFInteger> encryptReferenceState(inTrailerState->QueryDirectObject("mEncryptReference"));
	mTrailerInformation.SetEncrypt((ObjectIDType)encryptReferenceState->GetValue());

	PDFObjectCastPtr<PDFDictionary> infoDictionaryState(inStateReader->QueryDictionaryObject(inTrailerState,"mInfoDictionary"));
	ReadTrailerInfoState(inStateReader,infoDictionaryState.GetPtr());

	PDFObjectCastPtr<PDFInteger> infoDictionaryReferenceState(inTrailerState->QueryDirectObject("mInfoDictionaryReference"));
	mTrailerInformation.SetInfoDictionaryReference((ObjectIDType)infoDictionaryReferenceState->GetValue());

}

void DocumentContext::ReadTrailerInfoState(PDFParser* inStateReader,PDFDictionary* inTrailerInfoState)
{
	PDFObjectCastPtr<PDFLiteralString> titleState(inTrailerInfoState->QueryDirectObject("Title"));
	mTrailerInformation.GetInfo().Title = titleState->GetValue();

	PDFObjectCastPtr<PDFLiteralString> authorState(inTrailerInfoState->QueryDirectObject("Author"));
	mTrailerInformation.GetInfo().Author = authorState->GetValue();

	PDFObjectCastPtr<PDFLiteralString> subjectState(inTrailerInfoState->QueryDirectObject("Subject"));
	mTrailerInformation.GetInfo().Subject = subjectState->GetValue();

	PDFObjectCastPtr<PDFLiteralString> keywordsState(inTrailerInfoState->QueryDirectObject("Keywords"));
	mTrailerInformation.GetInfo().Keywords = keywordsState->GetValue();

	PDFObjectCastPtr<PDFLiteralString> creatorState(inTrailerInfoState->QueryDirectObject("Creator"));
	mTrailerInformation.GetInfo().Creator = creatorState->GetValue();

	PDFObjectCastPtr<PDFLiteralString> producerState(inTrailerInfoState->QueryDirectObject("Producer"));
	mTrailerInformation.GetInfo().Producer = producerState->GetValue();

	PDFObjectCastPtr<PDFDictionary> creationDateState(inTrailerInfoState->QueryDirectObject("CreationDate"));
	ReadDateState(creationDateState.GetPtr(),mTrailerInformation.GetInfo().CreationDate);

	PDFObjectCastPtr<PDFDictionary> modDateState(inTrailerInfoState->QueryDirectObject("ModDate"));
	ReadDateState(creationDateState.GetPtr(),mTrailerInformation.GetInfo().ModDate);

	PDFObjectCastPtr<PDFInteger> trappedState(inTrailerInfoState->QueryDirectObject("Trapped"));
	mTrailerInformation.GetInfo().Trapped = (EInfoTrapped)trappedState->GetValue();

	PDFObjectCastPtr<PDFDictionary> additionalInfoState(inTrailerInfoState->QueryDirectObject("mAdditionalInfoEntries"));

	MapIterator<PDFNameToPDFObjectMap> it = additionalInfoState->GetIterator();
	PDFObjectCastPtr<PDFName> keyState;
	PDFObjectCastPtr<PDFLiteralString> valueState;

	mTrailerInformation.GetInfo().ClearAdditionalInfoEntries();
	while(it.MoveNext())
	{
		keyState = it.GetKey();
		valueState = it.GetValue();

		mTrailerInformation.GetInfo().AddAdditionalInfoEntry(keyState->GetValue(),PDFTextString(valueState->GetValue()));
	}
}

void DocumentContext::ReadDateState(PDFDictionary* inDateState,PDFDate& inDate)
{
	PDFObjectCastPtr<PDFInteger> yearState(inDateState->QueryDirectObject("Year"));
	inDate.Year = (int)yearState->GetValue();

	PDFObjectCastPtr<PDFInteger> monthState(inDateState->QueryDirectObject("Month"));
	inDate.Month = (int)monthState->GetValue();

	PDFObjectCastPtr<PDFInteger> dayState(inDateState->QueryDirectObject("Day"));
	inDate.Day = (int)dayState->GetValue();

	PDFObjectCastPtr<PDFInteger> hourState(inDateState->QueryDirectObject("Hour"));
	inDate.Hour = (int)hourState->GetValue();

	PDFObjectCastPtr<PDFInteger> minuteState(inDateState->QueryDirectObject("Minute"));
	inDate.Minute = (int)minuteState->GetValue();

	PDFObjectCastPtr<PDFInteger> secondState(inDateState->QueryDirectObject("Second"));
	inDate.Second = (int)secondState->GetValue();

	PDFObjectCastPtr<PDFInteger> utcState(inDateState->QueryDirectObject("UTC"));
	inDate.UTC = (PDFDate::EUTCRelation)utcState->GetValue();

	PDFObjectCastPtr<PDFInteger> hourFromUTCState(inDateState->QueryDirectObject("HourFromUTC"));
	inDate.HourFromUTC = (int)hourFromUTCState->GetValue();

	PDFObjectCastPtr<PDFInteger> minuteFromUTCState(inDateState->QueryDirectObject("MinuteFromUTC"));
	inDate.MinuteFromUTC = (int)minuteFromUTCState->GetValue();
}

void DocumentContext::ReadCatalogInformationState(PDFParser* inStateReader,PDFDictionary* inCatalogInformationState)
{
	PDFObjectCastPtr<PDFIndirectObjectReference> pageTreeRootState(inCatalogInformationState->QueryDirectObject("PageTreeRoot"));

	// clear current state
	if(mCatalogInformation.GetCurrentPageTreeNode())
	{
		delete mCatalogInformation.GetPageTreeRoot(mObjectsContext->GetInDirectObjectsRegistry());
		mCatalogInformation.SetCurrentPageTreeNode(NULL);
	}


	if(!pageTreeRootState) // no page nodes yet...
		return;

	PDFObjectCastPtr<PDFIndirectObjectReference> currentPageTreeState(inCatalogInformationState->QueryDirectObject("mCurrentPageTreeNode"));
	mCurrentPageTreeIDInState = currentPageTreeState->mObjectID;

	PDFObjectCastPtr<PDFDictionary> pageTreeState(inStateReader->ParseNewObject(pageTreeRootState->mObjectID));
	
	PDFObjectCastPtr<PDFInteger> pageTreeIDState(pageTreeState->QueryDirectObject("mPageTreeID"));
	PageTree* rootNode = new PageTree((ObjectIDType)pageTreeIDState->GetValue());

	if(pageTreeRootState->mObjectID == mCurrentPageTreeIDInState)
		mCatalogInformation.SetCurrentPageTreeNode(rootNode);
	ReadPageTreeState(inStateReader,pageTreeState.GetPtr(),rootNode);

}

void DocumentContext::ReadPageTreeState(PDFParser* inStateReader,PDFDictionary* inPageTreeState,PageTree* inPageTree)
{
	PDFObjectCastPtr<PDFBoolean> isLeafParentState(inPageTreeState->QueryDirectObject("mIsLeafParent"));
	bool isLeafParent = isLeafParentState->GetValue();
		
	if(isLeafParent)
	{
		PDFObjectCastPtr<PDFArray> kidsIDsState(inPageTreeState->QueryDirectObject("mKidsIDs"));
		PDFObjectCastPtr<PDFInteger> kidID;
		
		SingleValueContainerIterator<PDFObjectVector> it = kidsIDsState->GetIterator();
		while(it.MoveNext())
		{
			kidID = it.GetItem();
			inPageTree->AddNodeToTree((ObjectIDType)kidID->GetValue(),mObjectsContext->GetInDirectObjectsRegistry());
		}
	}
	else
	{
		PDFObjectCastPtr<PDFArray> kidsNodesState(inPageTreeState->QueryDirectObject("mKidsNodes"));

		SingleValueContainerIterator<PDFObjectVector> it = kidsNodesState->GetIterator();
		while(it.MoveNext())
		{
			PDFObjectCastPtr<PDFDictionary> kidNodeState(inStateReader->ParseNewObject(((PDFIndirectObjectReference*)it.GetItem())->mObjectID));

			PDFObjectCastPtr<PDFInteger> pageTreeIDState(kidNodeState->QueryDirectObject("mPageTreeID"));
			PageTree* kidNode = new PageTree((ObjectIDType)pageTreeIDState->GetValue());

			if(((PDFIndirectObjectReference*)it.GetItem())->mObjectID == mCurrentPageTreeIDInState)
				mCatalogInformation.SetCurrentPageTreeNode(kidNode);
			ReadPageTreeState(inStateReader,kidNodeState.GetPtr(),kidNode);

			inPageTree->AddNodeToTree(kidNode,mObjectsContext->GetInDirectObjectsRegistry());
		}
	}
}

PDFDocumentCopyingContext* DocumentContext::CreatePDFCopyingContext(const wstring& inFilePath)
{
	PDFDocumentCopyingContext* context = new PDFDocumentCopyingContext();

	if(context->Start(inFilePath,this,mObjectsContext) != eSuccess)
	{
		delete context;
		return NULL;
	}
	else
		return context;
}

EStatusCode DocumentContext::AttachURLLinktoCurrentPage(const wstring& inURL,const PDFRectangle& inLinkClickArea)
{
	EStatusCodeAndObjectIDType writeResult = WriteAnnotationAndLinkForURL(inURL,inLinkClickArea);

	if(writeResult.first != eSuccess)
		return writeResult.first;

	RegisterAnnotationReferenceForNextPageWrite(writeResult.second);
	return eSuccess;
}

static const string scAnnot = "Annot";
static const string scLink = "Link";
static const string scRect = "Rect";
static const string scF = "F";
static const string scW = "W";
static const string scA = "A";
static const string scBS = "BS";
static const string scAction = "Action";
static const string scS = "S";
static const string scURI = "URI";
EStatusCodeAndObjectIDType DocumentContext::WriteAnnotationAndLinkForURL(const wstring& inURL,const PDFRectangle& inLinkClickArea)
{
	EStatusCodeAndObjectIDType result(eFailure,0);

	do
	{
		Ascii7Encoding encoding;

		BoolAndString encodedResult = encoding.Encode(inURL);
		if(!encodedResult.first)
		{
			TRACE_LOG1("DocumentContext::WriteAnnotationAndLinkForURL, unable to encode string to Ascii7. make sure that all charachters are valid URLs [should be ascii 7 compatible]. URL - %s",inURL.c_str());
			break;
		}

		result.second = mObjectsContext->StartNewIndirectObject();
		DictionaryContext* linkAnnotationContext = mObjectsContext->StartDictionary();

		// Type
		linkAnnotationContext->WriteKey(scType);
		linkAnnotationContext->WriteNameValue(scAnnot);

		// Subtype
		linkAnnotationContext->WriteKey(scSubType);
		linkAnnotationContext->WriteNameValue(scLink);

		// Rect
		linkAnnotationContext->WriteKey(scRect);
		linkAnnotationContext->WriteRectangleValue(inLinkClickArea);

		// F
		linkAnnotationContext->WriteKey(scF);
		linkAnnotationContext->WriteIntegerValue(4);

		// BS
		linkAnnotationContext->WriteKey(scBS);
		DictionaryContext* borderStyleContext = mObjectsContext->StartDictionary();

		borderStyleContext->WriteKey(scW);
		borderStyleContext->WriteIntegerValue(0);
		mObjectsContext->EndDictionary(borderStyleContext);

		// A
		linkAnnotationContext->WriteKey(scA);
		DictionaryContext* actionContext = mObjectsContext->StartDictionary();

		// Type
		actionContext->WriteKey(scType);
		actionContext->WriteNameValue(scAction);

		// S
		actionContext->WriteKey(scS);
		actionContext->WriteNameValue(scURI);

		// URI
		actionContext->WriteKey(scURI);
		actionContext->WriteLiteralStringValue(encodedResult.second);
		
		mObjectsContext->EndDictionary(actionContext);

		mObjectsContext->EndDictionary(linkAnnotationContext);
		mObjectsContext->EndIndirectObject();
		result.first = eSuccess;
	}while(false);

	return result;
}

void DocumentContext::RegisterAnnotationReferenceForNextPageWrite(ObjectIDType inAnnotationReference)
{
	mAnnotations.push_back(inAnnotationReference);
}

EStatusCode DocumentContext::MergePDFPagesToPage(PDFPage* inPage,
								const wstring& inPDFFilePath,
								const PDFPageRange& inPageRange,
								const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mPDFDocumentHandler.MergePDFPagesToPage(inPage,
												   inPDFFilePath,
												   inPageRange,
												   inCopyAdditionalObjects);
}

PDFImageXObject* DocumentContext::CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream)
{
	return mJPEGImageHandler.CreateImageXObjectFromJPGStream(inJPGStream);
}

PDFImageXObject* DocumentContext::CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inImageXObjectID)
{
	return mJPEGImageHandler.CreateImageXObjectFromJPGStream(inJPGStream,inImageXObjectID);
}

PDFFormXObject* DocumentContext::CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream)
{
	return mJPEGImageHandler.CreateFormXObjectFromJPGStream(inJPGStream);

}

PDFFormXObject* DocumentContext::CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inFormXObjectID)
{
	return mJPEGImageHandler.CreateFormXObjectFromJPGStream(inJPGStream,inFormXObjectID);
}