#include "PDFFormXObject.h"
#include "PDFStream.h"
#include "XObjectContentContext.h"
#include "ObjectsContext.h"

PDFFormXObject::PDFFormXObject(ObjectIDType inFormXObjectID,PDFStream* inXObjectStream,ObjectIDType inFormXObjectResourcesDictionaryID)
{
	mXObjectID = inFormXObjectID;
	mResourcesDictionaryID = inFormXObjectResourcesDictionaryID;
	mContentStream = inXObjectStream;
	mContentContext = new XObjectContentContext(this);	
}

PDFFormXObject::~PDFFormXObject(void)
{
	delete mContentStream;
	delete mContentContext;
}

ObjectIDType PDFFormXObject::GetObjectID()
{
	return mXObjectID;
}

ObjectIDType PDFFormXObject::GetResourcesDictionaryObjectID()
{
	return mResourcesDictionaryID;
}

ResourcesDictionary& PDFFormXObject::GetResourcesDictionary()
{
	return mResources;
}

PDFStream* PDFFormXObject::GetContentStream()
{
	return mContentStream;
}

XObjectContentContext* PDFFormXObject::GetContentContext()
{
	return mContentContext;
}