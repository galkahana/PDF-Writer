#include "PDFArray.h"

PDFArray::PDFArray(void) : PDFObject(eType)
{
}

PDFArray::~PDFArray(void)
{
	PDFObjectVector::iterator it = mValues.begin();

	for(; it != mValues.end(); ++it)
		(*it)->Release();
}

void PDFArray::AppendObject(PDFObject* inObject)
{
	inObject->AddRef();
	mValues.push_back(inObject);
}


SingleValueContainerIterator<PDFObjectVector> PDFArray::GetIterator()
{
	return SingleValueContainerIterator<PDFObjectVector>(mValues);
}

PDFObject* PDFArray::QueryObject(unsigned long i)
{
	if(mValues.size() <= i)
	{
		return NULL;
	}
	else
	{
		mValues[i]->AddRef();
		return mValues[i];
	}
}

unsigned long PDFArray::GetLength()
{
	return (unsigned long)mValues.size();
}