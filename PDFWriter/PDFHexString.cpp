#include "PDFHexString.h"

PDFHexString::PDFHexString(const string& inValue):PDFObject(ePDFObjectHexString)
{
	mValue = inValue;
}

PDFHexString::~PDFHexString(void)
{
}

const string& PDFHexString::GetValue() const
{
	return mValue;
}

PDFHexString::operator string() const
{
	return mValue;
}