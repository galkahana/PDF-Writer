#pragma once

#include <string>

class PDFUsedFont;

class GraphicState
{
public:
	GraphicState(void);
	GraphicState(const GraphicState& inGraphicState);
	~GraphicState(void);

	GraphicState& operator=(const GraphicState& inGraphicState);

	// current font properties
	PDFUsedFont* mFont;
	double mFontSize;
	std::string mPlacedFontName;



};
