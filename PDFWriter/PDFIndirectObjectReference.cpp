#include "PDFIndirectObjectReference.h"

PDFIndirectObjectReference::PDFIndirectObjectReference(ObjectIDType inObjectID,unsigned long inVersion):PDFObject(eType)
{
	mObjectID = inObjectID;
	mVersion = inVersion;
}

PDFIndirectObjectReference::~PDFIndirectObjectReference(void)
{
}
