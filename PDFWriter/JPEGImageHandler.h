#pragma once

#include "JPEGImageInformation.h"

#include <map>
#include <string>
#include <utility>

using namespace std;

class ObjectsContext;
class PDFImageXObject;
class IDocumentContextExtender;

typedef map<wstring,JPEGImageInformation> WStringToJPEGImageInformationMap;
typedef std::pair<bool,JPEGImageInformation> BoolAndJPEGImageInformation;


class JPEGImageHandler
{
public:
	JPEGImageHandler();
	~JPEGImageHandler(void);
	

	// use this for retrieving image information for JPEG (useful for deciphering JPG dimensions tags)
	BoolAndJPEGImageInformation RetrieveImageInformation(const wstring& inJPGFilePath);

	// DocumentContext::CreateImageXObjectFromJPGFile are equivelent
	PDFImageXObject* CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath);

	void SetObjectsContext(ObjectsContext* inObjectsContext);
	void SetDocumentContextExtender(IDocumentContextExtender* inExtender);

private:
	JPEGImageInformation mNullInformation;
	WStringToJPEGImageInformationMap mImagesInformationMap;
	ObjectsContext* mObjectsContext;
	IDocumentContextExtender* mExtender;

	PDFImageXObject* CreateAndWriteImageXObjectFromJPGInformation(const wstring& inJPGFilePath, const JPEGImageInformation& inJPGImageInformation);

};
