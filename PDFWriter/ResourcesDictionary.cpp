#include "ResourcesDictionary.h"
#include "BoxingBase.h"

ResourcesDictionary::ResourcesDictionary(void)
{
}

ResourcesDictionary::~ResourcesDictionary(void)
{
}

void ResourcesDictionary::AddProcsetResource(const string& inResourceName)
{
	if(mProcsets.find(inResourceName) == mProcsets.end())
		mProcsets.insert(inResourceName);
}

int ResourcesDictionary::GetProcsetsCount()
{
	return (int)mProcsets.size();
}

SingleValueContainerIterator<StringSet> ResourcesDictionary::GetProcesetsIterator()
{
	return SingleValueContainerIterator<StringSet>(mProcsets);
}

static const string scFM = "Fm";
string ResourcesDictionary::AddFormXObjectMapping(ObjectIDType inFormXObjectID)
{
	ObjectIDTypeToStringMap::iterator it = mFormXObjects.find(inFormXObjectID);

	if(it == mFormXObjects.end())
	{
		string newName = scFM + Int((int)mFormXObjects.size()+1).ToString();
		mFormXObjects.insert(ObjectIDTypeToStringMap::value_type(inFormXObjectID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

void ResourcesDictionary::AddFormXObjectMapping(ObjectIDType inFormXObjectID,const string& inFormXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mFormXObjects.find(inFormXObjectID);

	if(it == mFormXObjects.end())
		mFormXObjects.insert(ObjectIDTypeToStringMap::value_type(inFormXObjectID,inFormXObjectName));
	else
		it->second = inFormXObjectName;
}


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetFormXObjectsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mFormXObjects);
}

int ResourcesDictionary::GetFormXObjectsCount()
{
	return (int)mFormXObjects.size();
}

static const string scIM = "Im";
string ResourcesDictionary::AddImageXObjectMapping(ObjectIDType inImageXObjectID)
{
	ObjectIDTypeToStringMap::iterator it = mImageXObjects.find(inImageXObjectID);

	if(it == mImageXObjects.end())
	{
		string newName = scIM + Int((int)mImageXObjects.size()+1).ToString();
		mImageXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObjectID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}


void ResourcesDictionary::AddImageXObjectMapping(ObjectIDType inImageXObjectID, const string& inImageXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mImageXObjects.find(inImageXObjectID);

	if(it == mImageXObjects.end())
		mImageXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObjectID,inImageXObjectName));
	else
		it->second = inImageXObjectName;
}

int ResourcesDictionary::GetImageXObjectsCount()
{
	return (int)mImageXObjects.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetImageXObjectsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mImageXObjects);
}

int ResourcesDictionary::GetXObjectsCount()
{
	return int(mImageXObjects.size() + mFormXObjects.size());
}