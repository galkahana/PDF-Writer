#include "Timer.h"

Timer::Timer(void)
{
	mStartTime = 0;
	mTotal = 0;
}

Timer::~Timer(void)
{
}

void Timer::Reset()
{
	mStartTime = 0;
	mTotal = 0;
}

void Timer::StartMeasure()
{
	mStartTime = clock();
}

void Timer::StopMeasureAndAccumulate()
{
	mTotal += (double)(clock() - mStartTime)*1000/(CLOCKS_PER_SEC);
	mStartTime = 0;
}

double Timer::GetTotalMiliSeconds()
{
	return mTotal;
}	
