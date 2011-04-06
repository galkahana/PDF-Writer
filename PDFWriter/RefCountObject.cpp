#include "RefCountObject.h"

RefCountObject::RefCountObject(void)
{
	mRefCount = 1;
}

RefCountObject::~RefCountObject(void)
{
}

void RefCountObject::AddRef()
{
	++mRefCount;
}

void RefCountObject::Release()
{
	if(0 == mRefCount)
		return; // exception
	--mRefCount;
	if(0 == mRefCount)
		delete this;
}
