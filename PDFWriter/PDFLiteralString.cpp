#include "PDFLiteralString.h"

PDFLiteralString::PDFLiteralString(const string& inValue):PDFObject(eType)
{
	mValue = inValue;
}

PDFLiteralString::~PDFLiteralString(void)
{
}

const string& PDFLiteralString::GetValue() const
{
	return mValue;
}

PDFLiteralString::operator string() const
{
	return mValue;
}
