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
		mCurrentStream = mObjectsContext->CreatePDFStream();
		SetPDFStreamForWrite(mCurrentStream);
	}
}

ResourcesDictionary* PageContentContext::GetResourcesDictionary()
{
	return &(mPageOfContext->GetResourcesDictionary());
}

EStatusCode PageContentContext::FinalizeCurrentStream()
{
	if(mCurrentStream)
		return WriteCurrentStreamToMainStreamAndRelease();
	else
		return eSuccess;
}

EStatusCode PageContentContext::WriteCurrentStreamToMainStreamAndRelease()
{
	EStatusCode status;

	do
	{
		ObjectIDType streamObjectID = mObjectsContext->StartNewIndirectObject();
		mPageOfContext->AddContentStreamReference(streamObjectID);

		status = mObjectsContext->WritePDFStream(mCurrentStream);
		if(status != eSuccess)
		{
			TRACE_LOG("Failed to write page content stream");
			break;
		}

		mObjectsContext->EndIndirectObject();
		delete mCurrentStream;
		mCurrentStream = NULL;
		
	}while(false);

	return status;
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
