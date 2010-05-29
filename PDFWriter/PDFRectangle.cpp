#include "PDFRectangle.h"

PDFRectangle::PDFRectangle(void)
{
	LowerLeftX = 0;
	LowerLeftY = 0;
	UpperRightX = 0;
	UpperRightY = 0;
}

PDFRectangle::~PDFRectangle(void)
{
}

PDFRectangle::PDFRectangle(double inLowerLeftX, double inLowerLeftY, double inUpperRightX, double inUpperRightY)
{
	LowerLeftX = inLowerLeftX;
	LowerLeftY = inLowerLeftY;
	UpperRightX = inUpperRightX;
	UpperRightY = inUpperRightY;
}

PDFRectangle::PDFRectangle(const PDFRectangle& inOther)
{
	LowerLeftX = inOther.LowerLeftX;
	LowerLeftY = inOther.LowerLeftY;
	UpperRightX = inOther.UpperRightX;
	UpperRightY = inOther.UpperRightY;
}
