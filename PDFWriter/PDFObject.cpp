#include "PDFObject.h"

PDFObject::PDFObject(ePDFObjectType inType)
{
	mType = inType;
}

PDFObject::~PDFObject(void)
{
}

ePDFObjectType PDFObject::GetType()
{
	return mType;
}
