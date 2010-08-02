#include "PDFImageXObject.h"

PDFImageXObject::PDFImageXObject(ObjectIDType inImageObjectID)
{
	mImageObjectID = inImageObjectID;
}

PDFImageXObject::PDFImageXObject(ObjectIDType inImageObjectID,const std::string& inRequiredProcsetResourceName)
{
	mImageObjectID = inImageObjectID;
	AddRequiredProcset(inRequiredProcsetResourceName);
}

PDFImageXObject::~PDFImageXObject(void)
{
}

ObjectIDType PDFImageXObject::GetImageObjectID()
{
	return mImageObjectID;
}

const StringList& PDFImageXObject::GetRequiredProcsetResourceNames() const
{
	return mRequiredProcsetResourceNames;
}

void PDFImageXObject::AddRequiredProcset(const std::string& inRequiredProcsetResourceName)
{
	mRequiredProcsetResourceNames.push_back(inRequiredProcsetResourceName);
}
