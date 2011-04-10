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

void PDFDictionary::Insert(PDFName* inKeyObject, PDFObject* inValueObject)
{
	inKeyObject->AddRef();
	inValueObject->AddRef();

	mValues.insert(PDFNameToPDFObjectMap::value_type(inKeyObject,inValueObject));
}


bool PDFDictionary::Exists(string inName)
{
	PDFName key(inName);
	return mValues.find(&key) != mValues.end();
}

MapIterator<PDFNameToPDFObjectMap> PDFDictionary::GetIterator()
{
	return MapIterator<PDFNameToPDFObjectMap>(mValues);
}