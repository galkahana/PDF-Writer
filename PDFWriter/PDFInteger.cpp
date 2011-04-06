#include "PDFInteger.h"

PDFInteger::PDFInteger(long inValue):PDFObject(eType)
{
	mValue = inValue;
}

PDFInteger::~PDFInteger(void)
{
}

long PDFInteger::GetValue() const
{
	return mValue;
}

PDFInteger::operator long() const
{
	return mValue;
}

