#pragma once
#include "PDFObject.h"

#include <vector>

using namespace std;

typedef vector<PDFObject*> PDFObjectVector;

class PDFArray : public PDFObject
{
public:
	PDFArray(void);
	virtual ~PDFArray(void);
	
	// being lazy ;) just giving you some smart PTRs to the array itself
	PDFObjectVector* operator ->();

private:
	PDFObjectVector mValues;
};
