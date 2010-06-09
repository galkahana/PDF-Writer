#pragma once

#include "AbstractContentContext.h"

class PDFPage;
class ObjectsContext;
class PDFStream;

class PageContentContext : public AbstractContentContext
{
public:
	PageContentContext(PDFPage* inPageOfContext,ObjectsContext* inObjectsContext);
	virtual ~PageContentContext(void);

	// Finish writing a current stream, if exists and flush to the main PDF stream
	EStatusCode FinalizeCurrentStream();

	// Extensibility method, retrieves the current content stream for writing. if one does not exist - creates it.
	PDFStream* GetCurrentPageContentStream();

private:
	PDFPage* mPageOfContext;
	ObjectsContext* mObjectsContext;
	PDFStream* mCurrentStream;

	EStatusCode FinalizeStreamWriteAndRelease();
	void StartAStreamIfRequired();
	void StartContentStreamDefinition();

	// AbstractContentContext implementation
	virtual ResourcesDictionary* GetResourcesDictionary();
	virtual void RenewStreamConnection();

};
