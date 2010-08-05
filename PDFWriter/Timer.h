#pragma once

#include <ctime>

class Timer
{
public:
	Timer(void);
	~Timer(void);

	void Reset();
	void StartMeasure();
	void StopMeasureAndAccumulate();

	double GetTotalMiliSeconds();
private:
	clock_t mStartTime;
	double mTotal;

};
