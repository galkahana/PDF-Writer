#pragma once
#include "PDFObject.h"

class PDFReal : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectReal
	};

	PDFReal(double inValue);
	virtual ~PDFReal(void);

	double GetValue() const;
	operator double() const;

private:
	double mValue;
};
