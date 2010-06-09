#pragma once

#include "ObjectsBasicTypes.h"
#include "ResourcesDictionary.h"

class PDFStream;
class XObjectContentContext;
class ObjectsContext;

class PDFFormXObject
{
public:

	PDFFormXObject(ObjectIDType inFormXObjectID,PDFStream* inXObjectStream,ObjectIDType inFormXObjectResourcesDictionaryID);
	~PDFFormXObject(void);

	ObjectIDType GetObjectID();
	ObjectIDType GetResourcesDictionaryObjectID();

	ResourcesDictionary& GetResourcesDictionary();
	XObjectContentContext* GetContentContext();
	PDFStream* GetContentStream();

	
private:

	ObjectIDType mXObjectID;
	ObjectIDType mResourcesDictionaryID;
	ResourcesDictionary mResources;
	PDFStream* mContentStream;
	XObjectContentContext* mContentContext;
};
