#include "XObjectContentContext.h"
#include "PDFFormXObject.h"

XObjectContentContext::XObjectContentContext(PDFFormXObject* inFormXObject)
{
	mPDFFormXObjectOfContext = inFormXObject;
	SetPDFStreamForWrite(inFormXObject->GetContentStream());
}

XObjectContentContext::~XObjectContentContext(void)
{
}

ResourcesDictionary* XObjectContentContext::GetResourcesDictionary()
{
	return &(mPDFFormXObjectOfContext->GetResourcesDictionary());
}
