#pragma once
#include "PDFObject.h"
#include "IOBasicTypes.h"

class PDFDictionary;

using namespace IOBasicTypes;

class PDFStreamInput : public PDFObject
{
public:
	enum EType
	{
		eType = ePDFObjectStream
	};

	PDFStreamInput(PDFDictionary* inStreamDictionary,LongFilePositionType inStreamContentStart);
	virtual ~PDFStreamInput(void);

	PDFDictionary* GetStreamDictionary();
	PDFObject* GetExtentObject();
	LongFilePositionType GetStreamContentStart();

private:
	PDFDictionary* mDictionary;
	LongFilePositionType mStreamContentStart;
};
