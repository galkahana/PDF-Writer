#pragma once
#include "PDFObject.h"

class PDFNull : public PDFObject
{
public:
	enum EType
	{
		eType = ePDFObjectNull
	};

	PDFNull(void);
	virtual ~PDFNull(void);
};
