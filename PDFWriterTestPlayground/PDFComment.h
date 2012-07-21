#pragma once

// allowing myself a shortcut here for the color type and date type. real life sample should use only model classes...and not of the PDF writing implementation...
#include "PDFDate.h"
#include "CMYKRGBColor.h"

#include <string>


using namespace std;

class PDFComment
{
public:
	PDFComment(void);
	~PDFComment(void);

	string Text;
	string CommentatorName;
	double FrameBoundings[4];
	CMYKRGBColor Color;
	PDFDate Time;
	PDFComment* ReplyTo;

};
