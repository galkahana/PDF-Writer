#include "GraphicState.h"

#ifndef NULL
#define NULL 0
#endif

GraphicState::GraphicState(void)
{
	mFont = NULL;
	mFontSize = 0;
}

GraphicState::GraphicState(const GraphicState& inGraphicState)
{
	*this = inGraphicState;
}

GraphicState::~GraphicState(void)
{
}

GraphicState& GraphicState::operator=(const GraphicState& inGraphicState)
{
	mFont = inGraphicState.mFont;
	mFontSize = inGraphicState.mFontSize;
	mPlacedFontName = inGraphicState.mPlacedFontName;
	return *this;

}