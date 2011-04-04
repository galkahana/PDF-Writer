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

PDFObject* PDFDictionary::GetObject(string inName)
{
	PDFName key(inName);
	PDFNameToPDFObjectMap::iterator it = mValues.find(&key);

	if(it == mValues.end())
		return NULL;
	else
		return it->second;
}

