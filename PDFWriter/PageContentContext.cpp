/*
   Source File : PageContentContext.cpp


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
#include "PageContentContext.h"
#include "PDFPage.h"
#include "ObjectsContext.h"
#include "PDFStream.h"
#include "Trace.h"
#include "IPageEndWritingTask.h"
#include "DocumentContext.h"

using namespace PDFHummus;


PageContentContext::PageContentContext(PDFHummus::DocumentContext* inDocumentContext,PDFPage* inPageOfContext,ObjectsContext* inObjectsContext):AbstractContentContext(inDocumentContext)
{
	mPageOfContext = inPageOfContext;
	mObjectsContext = inObjectsContext;
	mCurrentStream = NULL;
}

PageContentContext::~PageContentContext(void)
{
}

EStatusCode PageContentContext::StartAStreamIfRequired()
{
    EStatusCode status = eSuccess;
	if(!mCurrentStream)
	{
		status = StartContentStreamDefinition();
        if(status != eSuccess)
        {
            TRACE_LOG("PageContentContext::StartAStreamIfRequired, Unexpected Error, failed to start a new content stream object");
            return status;
        }
		mCurrentStream = mObjectsContext->StartPDFStream();
		SetPDFStreamForWrite(mCurrentStream);
	}
    return status;
}

EStatusCode PageContentContext::StartContentStreamDefinition()
{
	ObjectIDType streamObjectID = mObjectsContext->StartNewIndirectObject();
    if(0 == streamObjectID)
    {
        TRACE_LOG("PageContentContext::StartContentStreamDefinition, Unexpected Error, failed to start a new stream object");
        return eFailure;
    }
	mPageOfContext->AddContentStreamReference(streamObjectID);
    return eSuccess;
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
		return PDFHummus::eSuccess;
}

PDFPage* PageContentContext::GetAssociatedPage()
{
	return mPageOfContext;
}

EStatusCode PageContentContext::FinalizeStreamWriteAndRelease()
{
	EStatusCode status = mObjectsContext->EndPDFStream(mCurrentStream);

	delete mCurrentStream; 
	mCurrentStream = NULL;
	return status;
}

PDFStream* PageContentContext::GetCurrentPageContentStream()
{
	if(StartAStreamIfRequired() != eSuccess)
    {
        TRACE_LOG("PageContentContext::GetCurrentPageContentStream, Unexpected Error, failed to start a new content stream object");
        return NULL;
    }
	return mCurrentStream;	
}

EStatusCode PageContentContext::RenewStreamConnection()
{
	return StartAStreamIfRequired();
}

class PageImageWritingTask : public IPageEndWritingTask
{
public:
    PageImageWritingTask(const std::string& inImagePath,unsigned long inImageIndex,ObjectIDType inObjectID,const PDFParsingOptions& inPDFParsingOptions)
    {mImagePath = inImagePath;mImageIndex = inImageIndex;mObjectID = inObjectID;mPDFParsingOptions = inPDFParsingOptions;}
    
    virtual ~PageImageWritingTask(){}
    
    virtual PDFHummus::EStatusCode Write(PDFPage* inPageObject,
                                         ObjectsContext* inObjectsContext,
                                         PDFHummus::DocumentContext* inDocumentContext)
    {
        return inDocumentContext->WriteFormForImage(mImagePath,mImageIndex,mObjectID,mPDFParsingOptions);
    }
    
private:
    std::string mImagePath;
    unsigned long mImageIndex;
    ObjectIDType mObjectID;
	PDFParsingOptions mPDFParsingOptions;
};

void PageContentContext::ScheduleImageWrite(const std::string& inImagePath,unsigned long inImageIndex,ObjectIDType inObjectID, const PDFParsingOptions& inParsingOptions)
{
    mDocumentContext->RegisterPageEndWritingTask(GetAssociatedPage(),
                                                 new PageImageWritingTask(inImagePath,inImageIndex,inObjectID,inParsingOptions));

}

void PageContentContext::ScheduleObjectEndWriteTask(IObjectEndWritingTask* inObjectEndWritingTask) 
{
    mDocumentContext->RegisterPageEndWritingTask(GetAssociatedPage(), inObjectEndWritingTask);
}
