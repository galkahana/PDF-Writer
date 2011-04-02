#include "PDFArray.h"

PDFArray::PDFArray(void) : PDFObject(ePDFObjectArray)
{
}

PDFArray::~PDFArray(void)
{
	PDFObjectVector::iterator it = mValues.begin();

	for(; it != mValues.end(); ++it)
		delete *it;
}

PDFObjectVector* PDFArray::operator ->()
{
	return &mValues;
}
