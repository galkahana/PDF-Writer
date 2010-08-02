#pragma once

// 256 values based color

class CMYKRGBColor
{
public:
	CMYKRGBColor(void);
	CMYKRGBColor(unsigned char inR,unsigned char inG, unsigned char inB);
	CMYKRGBColor(unsigned char inC,unsigned char inM, unsigned char inY, unsigned char inK);
	~CMYKRGBColor(void);

	bool UseCMYK ; // toggle RGB or CMYK
	unsigned char RGBComponents[3];
	unsigned char CMYKComponents[4];

	static const CMYKRGBColor CMYKBlack;
	static const CMYKRGBColor CMYKWhite;
};

