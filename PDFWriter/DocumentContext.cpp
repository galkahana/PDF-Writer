#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "IOBasicTypes.h"
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

DocumentContext::DocumentContext(void)
{
	mObjectsContext = NULL;
	mExtender = NULL;
}

DocumentContext::~DocumentContext(void)
{
}

void DocumentContext::SetObjectsContext(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
}

void DocumentContext::SetOutputFileInformation(OutputFile* inOutputFile)
{
	// just save the output file path for the ID generation in the end
	mOutputFilePath = inOutputFile->GetFilePath();
}

void DocumentContext::SetDocumentContextExtender(IDocumentContextExtender* inExtender)
{
	mExtender = inExtender;
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

	if(mExtender)
	{
		status = mExtender->OnCatalogWrite(&mCatalogInformation,catalogContext,mObjectsContext,this);
		if(status != eSuccess)
			TRACE_LOG("DocumentContext::WriteCatalogObject, unexpected faiulre. extender declared failure when writing catalog.");
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

static const string scPage = "Page";
static const string scMediaBox = "MediaBox";
static const string scContents = "Contents";

EStatusCode DocumentContext::WritePage(PDFPage* inPage)
{
	EStatusCode status = eSuccess;
	ObjectIDType pageID = mObjectsContext->StartNewIndirectObject();

	DictionaryContext* pageContext = mObjectsContext->StartDictionary();

	// type
	pageContext->WriteKey(scType);
	pageContext->WriteNameValue(scPage);

	// parent
	pageContext->WriteKey(scParent);
	pageContext->WriteObjectReferenceValue(mCatalogInformation.AddPageToPageTree(pageID,mObjectsContext->GetInDirectObjectsRegistry()));
	
	// Media Box
	pageContext->WriteKey(scMediaBox);
	pageContext->WriteRectangleValue(inPage->GetMediaBox());

	do
	{
		status = WriteResourcesDictionary(inPage->GetResourcesDictionary(),pageContext);
		if(status != eSuccess)
		{
			TRACE_LOG("DocumentContext::WritePage, failed to write resources dictionary");
			break;
		}

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

		if(mExtender)
		{
			status = mExtender->OnPageWrite(inPage,pageContext,mObjectsContext,this);
			if(status != eSuccess)
			{
				TRACE_LOG("DocumentContext::WritePage, unexpected faiulre. extender declared failure when writing page.");
				break;
			}
		}
		status = mObjectsContext->EndDictionary(pageContext);
		if(status != eSuccess)
		{
			TRACE_LOG("DocumentContext::WritePage, unexpected faiulre. Failed to end dictionary in page write.");
			break;
		}
		mObjectsContext->EndIndirectObject();	
	}while(false);

	return status;
}


EStatusCode DocumentContext::WritePageAndRelease(PDFPage* inPage)
{
	EStatusCode status = WritePage(inPage);
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

PageContentContext* DocumentContext::StartPageContentContext(PDFPage* inPage)
{
	return new PageContentContext(inPage,mObjectsContext);
}

EStatusCode DocumentContext::PausePageContentContext(PageContentContext* inPageContext)
{
	return inPageContext->FinalizeCurrentStream();
}

EStatusCode DocumentContext::EndPageContentContext(PageContentContext* inPageContext)
{
	EStatusCode status = inPageContext->FinalizeCurrentStream();
	delete inPageContext;
	return status;
}

PDFFormXObject* DocumentContext::CreateFormXObject()
{
	return new PDFFormXObject(mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());
}

static const string scResources = "Resources";
static const string scXObjects = "XObject";
static const string scProcesets = "ProcSet";
EStatusCode DocumentContext::WriteResourcesDictionary(ResourcesDictionary& inResourcesDictionary,DictionaryContext* inParentDictionaryContext)
{
	EStatusCode status = eSuccess;

	do
	{

		// Resource dict 
		inParentDictionaryContext->WriteKey(scResources);
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
			MapIterator<ObjectIDTypeToStringMap> itXObjects = inResourcesDictionary.GetFormXObjectsIterator();
			while(itXObjects.MoveNext())
			{
				xobjectsContext->WriteKey(itXObjects.GetValue());
				xobjectsContext->WriteObjectReferenceValue(itXObjects.GetKey());
			}
			mObjectsContext->EndDictionary(xobjectsContext);
		}

		if(mExtender)
		{
			status = mExtender->OnResourcesWrite(&(inResourcesDictionary),resourcesContext,mObjectsContext,this);
			if(status != eSuccess)
			{
				TRACE_LOG("DocumentContext::WriteResourcesDictionary, unexpected faiulre. extender declared failure when writing resources.");
				break;
			}
		}

		mObjectsContext->EndDictionary(resourcesContext); 
	}while(false);

	return status;
}

static const string scXObject = "XObject";
static const string scSubType = "Subtype";
static const string scForm = "Form";
static const string scBBox = "BBox";
static const string scFormType = "FormType";
static const string scMatrix = "Matrix";
EStatusCode DocumentContext::WriteFormXObjectAndRelease(PDFFormXObject* inFormXObject)
{
	EStatusCode status = eSuccess;
	mObjectsContext->StartNewIndirectObject(inFormXObject->GetObjectID());
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
	xobjectContext->WriteRectangleValue(inFormXObject->GetBoundingBox());

	// matrix
	if(!IsIdentityMatrix(inFormXObject->GetMatrix()))
	{
		xobjectContext->WriteKey(scMatrix);
		mObjectsContext->StartArray();
		for(int i=0;i<6;++i)
			mObjectsContext->WriteDouble(inFormXObject->GetMatrix()[i]);
		mObjectsContext->EndArray(eTokenSeparatorEndLine);
	}

	do
	{
		status = WriteResourcesDictionary(inFormXObject->GetResourcesDictionary(),xobjectContext);
		if(status != eSuccess)
		{
			TRACE_LOG("DocumentContext::WriteFormXObjectAndRelease, failed to write resources dictionary");
			break;
		}

		if(mExtender)
		{
			status = mExtender->OnFormXObjectWrite(inFormXObject,xobjectContext,mObjectsContext,this);
			if(status != eSuccess)
			{
				TRACE_LOG("DocumentContext::WriteFormXObjectAndRelease, unexpected faiulre. extender declared failure when writing form xobject.");
				break;
			}
		}

		status = mObjectsContext->WritePDFStream(inFormXObject->GetContentStream(),xobjectContext);
		if(status != eSuccess)
		{
			TRACE_LOG("DocumentContext::WriteFormXObjectAndRelease, failed to xobject stream");
			break;
		}
		
		// Important! no need to close the dictionary, because this is being handled by WritePDFStream already (which also wrote the stream later)

		mObjectsContext->EndIndirectObject();

		delete inFormXObject;
	}while(false);
	
	return status;
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