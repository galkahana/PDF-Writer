#pragma once
#include "PDFObject.h"

class PDFInteger : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectInteger
	};

	PDFInteger(long inValue);
	virtual ~PDFInteger(void);

	long GetValue() const;
	operator long() const;

private:
	long mValue;
};
