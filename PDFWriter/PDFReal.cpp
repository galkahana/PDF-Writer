#include "PDFReal.h"

PDFReal::PDFReal(double inValue):PDFObject(eType)
{
	mValue = inValue;
}

PDFReal::~PDFReal(void)
{
}

double PDFReal::GetValue() const
{
	return mValue;
}

PDFReal::operator double() const
{
	return mValue;
}
