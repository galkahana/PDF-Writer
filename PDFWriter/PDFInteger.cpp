#include "PDFInteger.h"

PDFInteger::PDFInteger(long inValue):PDFObject(ePDFObjectInteger)
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

