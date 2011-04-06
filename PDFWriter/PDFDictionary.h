#pragma once
#include "PDFObject.h"
#include "PDFName.h"

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

	// being lazy ;) just giving you some smart PTRs to the map itself
	PDFNameToPDFObjectMap* operator ->();

	PDFObject* QueryDirectObject(string inName);

private:

	PDFNameToPDFObjectMap mValues;
};
