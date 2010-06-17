#pragma once
#include "ObjectsBasicTypes.h"
#include "JPEGImageInformation.h"


class PDFImageXObject
{
public:
	PDFImageXObject(ObjectIDType inImageObjectID,const JPEGImageInformation& inImageInformation);
	~PDFImageXObject(void);

	ObjectIDType GetImageObjectID();
	const JPEGImageInformation& GetImageInformation() const;


private:

	ObjectIDType mImageObjectID;
	JPEGImageInformation mImageInformation;
};
