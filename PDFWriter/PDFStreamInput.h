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

	// These two calls AddRef on both objects
	PDFDictionary* QueryStreamDictionary();

	LongFilePositionType GetStreamContentStart();

private:
	PDFDictionary* mDictionary;
	LongFilePositionType mStreamContentStart;
};
