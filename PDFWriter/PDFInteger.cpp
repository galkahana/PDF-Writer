#include "PDFInteger.h"

PDFInteger::PDFInteger(long long inValue):PDFObject(eType)
{
	mValue = inValue;
}

PDFInteger::~PDFInteger(void)
{
}

long long PDFInteger::GetValue() const
{
	return mValue;
}

PDFInteger::operator long long() const
{
	return mValue;
}

