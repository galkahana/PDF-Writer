#pragma once

class PDFRectangle
{
public:
	PDFRectangle(void);
	~PDFRectangle(void);

	PDFRectangle(double inLowerLeftX, double inLowerLeftY, double inUpperRightX, double inUpperRightY);
	PDFRectangle(const PDFRectangle& inOther);

	double LowerLeftX;
	double LowerLeftY;
	double UpperRightX;
	double UpperRightY;

};
