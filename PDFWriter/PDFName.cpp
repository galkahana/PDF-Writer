#include "PDFName.h"

PDFName::PDFName(const string& inValue) : PDFObject(eType)
{
	mValue = inValue;
}

PDFName::~PDFName(void)
{
}

const string& PDFName::GetValue() const
{
	return mValue;
}

PDFName::operator string() const
{
	return mValue;
}
