#pragma once
#include "PDFObject.h"
#include "SingleValueContainerIterator.h"

#include <vector>

using namespace std;

typedef vector<PDFObject*> PDFObjectVector;

class PDFArray : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectArray
	};

	PDFArray(void);
	virtual ~PDFArray(void);

	// Will add to end, calls AddRef
	void AppendObject(PDFObject* inObject);
	
	// Returns an object for iterating the array
	SingleValueContainerIterator<PDFObjectVector> GetIterator();

	// Returns object at a given index, calls AddRef
	PDFObject* QueryObject(unsigned long i);
	unsigned long GetLength();

private:
	PDFObjectVector mValues;
};
