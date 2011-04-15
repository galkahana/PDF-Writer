#pragma once
#include "PDFObject.h"

class PDFInteger : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectInteger
	};

	PDFInteger(long long inValue);
	virtual ~PDFInteger(void);

	long long GetValue() const;
	operator long long() const;

private:
	long long mValue;
};
