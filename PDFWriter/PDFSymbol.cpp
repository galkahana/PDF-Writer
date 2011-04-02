#include "PDFSymbol.h"

PDFSymbol::PDFSymbol(const string& inSymbol) : PDFObject(ePDFObjectSymbol)
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
