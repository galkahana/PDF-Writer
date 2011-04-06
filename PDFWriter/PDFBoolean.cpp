#include "PDFBoolean.h"

PDFBoolean::PDFBoolean(bool inValue):PDFObject(eType)
{
	mValue = inValue;
}

PDFBoolean::~PDFBoolean(void)
{
}

bool PDFBoolean::GetValue() const
{
	return mValue;
}

PDFBoolean::operator bool() const
{
	return mValue;
}