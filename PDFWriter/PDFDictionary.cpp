#include "PDFDictionary.h"

PDFDictionary::PDFDictionary(void) : PDFObject(eType)
{ 
}

PDFDictionary::~PDFDictionary(void)
{
	PDFNameToPDFObjectMap::iterator	it = mValues.begin();

	for(; it != mValues.end(); ++it)
	{
		it->first->Release();
		it->second->Release();
	}
}

PDFNameToPDFObjectMap* PDFDictionary::operator ->()
{
	return &mValues;
}

PDFObject* PDFDictionary::QueryDirectObject(string inName)
{
	PDFName key(inName);
	PDFNameToPDFObjectMap::iterator it = mValues.find(&key);

	if(it == mValues.end())
	{
		return NULL;
	}
	else
	{
		it->second->AddRef();
		return it->second;
	}
}

