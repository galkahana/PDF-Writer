#include "PDFDictionary.h"

PDFDictionary::PDFDictionary(void) : PDFObject(ePDFObjectDictionary)
{ 
}

PDFDictionary::~PDFDictionary(void)
{
	PDFNameToPDFObjectMap::iterator	it = mValues.begin();

	for(; it != mValues.end(); ++it)
	{
		delete it->first;
		delete it->second;
	}
}

PDFNameToPDFObjectMap* PDFDictionary::operator ->()
{
	return &mValues;
}
