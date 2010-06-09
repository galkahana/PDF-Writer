#include "PageContentContext.h"
#include "PDFPage.h"
#include "ObjectsContext.h"
#include "PDFStream.h"
#include "Trace.h"

PageContentContext::PageContentContext(PDFPage* inPageOfContext,ObjectsContext* inObjectsContext)
{
	mPageOfContext = inPageOfContext;
	mObjectsContext = inObjectsContext;
	mCurrentStream = NULL;
}

PageContentContext::~PageContentContext(void)
{
}

void PageContentContext::StartAStreamIfRequired()
{
	if(!mCurrentStream)
	{
		StartContentStreamDefinition();
		mCurrentStream = mObjectsContext->StartPDFStream();
		SetPDFStreamForWrite(mCurrentStream);
	}
}

void PageContentContext::StartContentStreamDefinition()
{
	ObjectIDType streamObjectID = mObjectsContext->StartNewIndirectObject();
	mPageOfContext->AddContentStreamReference(streamObjectID);
}

ResourcesDictionary* PageContentContext::GetResourcesDictionary()
{
	return &(mPageOfContext->GetResourcesDictionary());
}

EStatusCode PageContentContext::FinalizeCurrentStream()
{
	if(mCurrentStream)
		return FinalizeStreamWriteAndRelease();
	else
		return eSuccess;
}

EStatusCode PageContentContext::FinalizeStreamWriteAndRelease()
{
	mObjectsContext->EndPDFStream(mCurrentStream);

	delete mCurrentStream;
	mCurrentStream = NULL;
	return eSuccess;
}

PDFStream* PageContentContext::GetCurrentPageContentStream()
{
	StartAStreamIfRequired();
	return mCurrentStream;	
}

void PageContentContext::RenewStreamConnection()
{
	StartAStreamIfRequired();
}
