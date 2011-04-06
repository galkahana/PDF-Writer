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

PDFObjectVector* PDFArray::operator ->()
{
	return &mValues;
}
