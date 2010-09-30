#pragma once

#include "GraphicState.h"
#include "EStatusCode.h"
#include <list>

typedef std::list<GraphicState> GraphicStateList;

class GraphicStateStack
{
public:
	GraphicStateStack(void);
	~GraphicStateStack(void);

	// push one level. following a "gsave" command, normally. new state copies old state variables
	void Push();

	// pop one level. following a "grestore" command. returns error if stack is underflow.
	EStatusCode Pop();

	GraphicState& GetCurrentState();

private:
	GraphicStateList mGraphicStateStack;
};
