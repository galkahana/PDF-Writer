#include "PDFImageXObject.h"

PDFImageXObject::PDFImageXObject(ObjectIDType inImageObjectID,const JPEGImageInformation& inImageInformation)
{
	mImageObjectID = inImageObjectID;
	mImageInformation = inImageInformation;
}

PDFImageXObject::~PDFImageXObject(void)
{
}

ObjectIDType PDFImageXObject::GetImageObjectID()
{
	return mImageObjectID;
}

const JPEGImageInformation& PDFImageXObject::GetImageInformation() const
{
	return mImageInformation;
}

