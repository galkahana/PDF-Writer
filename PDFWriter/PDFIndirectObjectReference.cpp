#include "PDFIndirectObjectReference.h"

PDFIndirectObjectReference::PDFIndirectObjectReference(ObjectIDType inObjectID,unsigned long inVersion):PDFObject(ePDFObjectIndirectObjectReference)
{
	mObjectID = inObjectID;
	mVersion = inVersion;
}

PDFIndirectObjectReference::~PDFIndirectObjectReference(void)
{
}
