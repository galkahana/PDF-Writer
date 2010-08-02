#pragma once
#include "ObjectsBasicTypes.h"
#include "JPEGImageInformation.h"

#include <string>
#include <list>

typedef std::list<std::string> StringList;

class PDFImageXObject
{
public:
	PDFImageXObject(ObjectIDType inImageObjectID);
	PDFImageXObject(ObjectIDType inImageObjectID,const std::string& inRequiredProcsetResourceName);
	~PDFImageXObject(void);

	ObjectIDType GetImageObjectID();
	const StringList& GetRequiredProcsetResourceNames() const;

	void AddRequiredProcset(const std::string& inRequiredProcsetResourceName);

private:

	ObjectIDType mImageObjectID;
	StringList mRequiredProcsetResourceNames;
};
