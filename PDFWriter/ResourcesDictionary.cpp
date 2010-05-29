#include "ResourcesDictionary.h"

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