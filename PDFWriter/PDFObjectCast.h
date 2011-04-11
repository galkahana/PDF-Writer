#pragma once

#include "RefCountPtr.h"
#include "PDFObject.h"

/*
	This small template function is only intended to be used for automatic casting of retrieved PDFObjects to their respective actual
	objects...and not for anything else.
*/ 

template <class T>
T* PDFObjectCast(PDFObject* inOriginal)
{
	if(!inOriginal)
		return NULL;

	if(inOriginal->GetType() == T::eType)
	{
		return (T*)inOriginal;
	}
	else
	{
		inOriginal->Release();
		return NULL;
	}
}

template <class T>
class PDFObjectCastPtr : public RefCountPtr<T>
{
public:
	PDFObjectCastPtr():RefCountPtr()
	{
	}

	PDFObjectCastPtr(PDFObject* inPDFObject): RefCountPtr(PDFObjectCast<T>(inPDFObject))
	{
	}

	PDFObjectCastPtr<T>&  operator =(PDFObject* inValue)
	{
		RefCountPtr::operator =(PDFObjectCast<T>(inValue));
		return *this;
	}
};


