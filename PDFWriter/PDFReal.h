#pragma once
#include "PDFObject.h"

class PDFReal : public PDFObject
{
public:
	PDFReal(double inValue);
	virtual ~PDFReal(void);

	double GetValue() const;
	operator double() const;

private:
	double mValue;
};
