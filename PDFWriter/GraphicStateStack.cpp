#include "GraphicStateStack.h"
#include "Trace.h"

GraphicStateStack::GraphicStateStack(void)
{
	mGraphicStateStack.push_back(GraphicState());
}

GraphicStateStack::~GraphicStateStack(void)
{
}

void GraphicStateStack::Push()
{
	GraphicState newState;
	
	newState = mGraphicStateStack.back(); // there's always at least one - which is the initial state
	mGraphicStateStack.push_back(newState);
}

EStatusCode GraphicStateStack::Pop()
{
	if(mGraphicStateStack.size() == 1)
	{
		TRACE_LOG("GraphicStateStack::Pop, exception. stack underflow, reached to the initial state");
		return eFailure;
	}
	else
	{
		mGraphicStateStack.pop_back();
		return eSuccess;
	}
}

GraphicState& GraphicStateStack::GetCurrentState()
{
	return mGraphicStateStack.back();
}