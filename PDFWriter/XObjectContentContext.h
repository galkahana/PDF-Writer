#pragma once
#include "AbstractContentContext.h"

class PDFFormXObject;

class XObjectContentContext : public AbstractContentContext
{
public:
	XObjectContentContext(PDFFormXObject* inFormXObject);
	virtual ~XObjectContentContext(void);

private:

	// AbstractContentContext implementation
	virtual ResourcesDictionary* GetResourcesDictionary();

	PDFFormXObject* mPDFFormXObjectOfContext;
};
