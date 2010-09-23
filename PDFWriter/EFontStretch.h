#pragma once

//aligning index to be like the Open Type width class numbers, for direct mapping

enum EFontStretch {
	eFontStretchUltraCondensed = 1, 
	eFontStretchExtraCondensed, 
	eFontStretchCondensed, 
	eFontStretchSemiCondensed, 
	eFontStretchNormal, 
	eFontStretchSemiExpanded, 
	eFontStretchExpanded, 
	eFontStretchExtraExpanded,
	eFontStretchUltraExpanded,
	eFontStretchMax,
	eFontStretchUknown
};

static const char* scFontStretchLabels[eFontStretchMax] =
{
	"",
	"UltraCondensed", 
	"ExtraCondensed", 
	"Condensed", 
	"SemiCondensed", 
	"Normal", 
	"SemiExpanded", 
	"Expanded", 
	"ExtraExpanded",
	"UltraExpanded"
};