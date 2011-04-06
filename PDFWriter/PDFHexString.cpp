#include "PDFHexString.h"

PDFHexString::PDFHexString(const string& inValue):PDFObject(eType)
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