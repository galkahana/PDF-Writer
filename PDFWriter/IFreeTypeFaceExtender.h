#pragma once

#include "EFontStretch.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H

#include <utility>

typedef std::pair<bool,FT_Short> BoolAndFTShort;

class IFreeTypeFaceExtender
{
public:
	virtual ~IFreeTypeFaceExtender(){}
	
	// Italic Angle - if you don't know...just return 0
	virtual	double GetItalicAngle() = 0;
	
	// CapHeight - bool should indicate whether the 2nd value is valid. this will indicate container to go look for the value by measuring the y value of H
	virtual BoolAndFTShort GetCapHeight() = 0;

	// XHeight - bool should indicae whether the 2nd value is valid. this will indicate container to go look for the value by measuring the y value of x
	virtual BoolAndFTShort GetxHeight() = 0;

	// Stem V, please do try and find a valid value...
	virtual FT_UShort GetStemV() = 0;

	// return eFontStretchUknown if you can't tell. will try to analyze from the font style name
	virtual EFontStretch GetFontStretch() = 0;

	// return 1000 if you can't tell. will try to analyze from font style name
	virtual FT_UShort GetFontWeight() = 0;

	// return a good answer or false if don't know. no fallback here
	virtual bool HasSerifs() = 0;

	// return a good answer or false if you don't know. no fallback here
	virtual bool IsForceBold() = 0;
};