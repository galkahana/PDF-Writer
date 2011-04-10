#pragma once
#include "PDFObject.h"
#include "PDFName.h"
#include "MapIterator.h"

#include <map>

using namespace std;

class PDFNameLess : public binary_function<const PDFName*,const PDFName*,bool>
{
public:
	bool operator( ) (const PDFName* left, 
						const PDFName* right ) const
	{
		return left->GetValue() < right->GetValue();
	}
};

typedef map<PDFName*,PDFObject*,PDFNameLess> PDFNameToPDFObjectMap;

class PDFDictionary : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectDictionary
	};

	PDFDictionary(void);
	virtual ~PDFDictionary(void);

	// AddRefs on both
	void Insert(PDFName* inKeyObject, PDFObject* inValueObject);

	bool Exists(string inName);
	PDFObject* QueryDirectObject(string inName);

	MapIterator<PDFNameToPDFObjectMap> GetIterator();

private:

	PDFNameToPDFObjectMap mValues;
};
