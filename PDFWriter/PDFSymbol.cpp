#include "PDFSymbol.h"

PDFSymbol::PDFSymbol(const string& inSymbol) : PDFObject(eType)
{
	mValue = inSymbol;
}

PDFSymbol::~PDFSymbol(void)
{
}

const string& PDFSymbol::GetValue() const
{
	return mValue;
}
