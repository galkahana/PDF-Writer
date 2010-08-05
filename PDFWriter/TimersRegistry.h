#pragma once

#include "Timer.h"

#include <string>
#include <map>

using namespace std;

typedef map<wstring,Timer> WStringToTimerMap;

class TimersRegistry
{
public:
	TimersRegistry(void);
	~TimersRegistry(void);

	void StartMeasure(const wstring& inTimerName);
	void StopMeasureAndAccumulate(const wstring& inTimerName);

	double GetTotalMiliSeconds(const wstring& inTimerName);

	Timer& GetTimer(const wstring& inTimerName);

	void ReleaseAll();
	void TraceAll();
	void TraceAndReleaseAll();

private:
	WStringToTimerMap mTimers;
};
