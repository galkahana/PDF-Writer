#include "PDFObject.h"

PDFObject::PDFObject(EPDFObjectType inType)
{
	mType = inType;
}

PDFObject::PDFObject(int inType)
{
	mType = (EPDFObjectType)inType;
}


PDFObject::~PDFObject(void)
{
}

EPDFObjectType PDFObject::GetType()
{
	return mType;
}
