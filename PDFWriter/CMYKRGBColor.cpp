#include "CMYKRGBColor.h"

CMYKRGBColor::CMYKRGBColor(void)
{
	UseCMYK = false;
	RGBComponents[0] = RGBComponents[1] = RGBComponents[2] = 0;
	CMYKComponents[0] = CMYKComponents[1] = CMYKComponents[2] = CMYKComponents[3] =0;
}

CMYKRGBColor::~CMYKRGBColor(void)
{
}


CMYKRGBColor::CMYKRGBColor(unsigned char inR,unsigned char inG, unsigned char inB)
{
	UseCMYK = false;
	RGBComponents[0] = inR;
	RGBComponents[1] = inG;
	RGBComponents[2] = inB;
	CMYKComponents[0] = CMYKComponents[1] = CMYKComponents[2] = CMYKComponents[3] =0;
}

CMYKRGBColor::CMYKRGBColor(unsigned char inC,unsigned char inM, unsigned char inY, unsigned char inK)
{
	UseCMYK = true;
	CMYKComponents[0] = inC;
	CMYKComponents[1] = inM;
	CMYKComponents[2] = inY;
	CMYKComponents[3] = inK;
	RGBComponents[0] = RGBComponents[1] = RGBComponents[2] = 0;
}

const CMYKRGBColor CMYKRGBColor::CMYKBlack(0,0,0,255);
const CMYKRGBColor CMYKRGBColor::CMYKWhite(0,0,0,0);
