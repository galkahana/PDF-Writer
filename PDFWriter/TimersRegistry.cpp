#include "TimersRegistry.h"
#include "Trace.h"

TimersRegistry::TimersRegistry(void)
{
}

TimersRegistry::~TimersRegistry(void)
{
}

void TimersRegistry::StartMeasure(const wstring& inTimerName)
{
	WStringToTimerMap::iterator it = mTimers.find(inTimerName);
	if(it == mTimers.end())
		it = mTimers.insert(WStringToTimerMap::value_type(inTimerName,Timer())).first;

	it->second.StartMeasure();
}

void TimersRegistry::StopMeasureAndAccumulate(const wstring& inTimerName)
{
	WStringToTimerMap::iterator it = mTimers.find(inTimerName);
	if(it == mTimers.end())
		it = mTimers.insert(WStringToTimerMap::value_type(inTimerName,Timer())).first;

	it->second.StopMeasureAndAccumulate();
}

double TimersRegistry::GetTotalMiliSeconds(const wstring& inTimerName)
{
	WStringToTimerMap::iterator it = mTimers.find(inTimerName);
	if(it == mTimers.end())
		it = mTimers.insert(WStringToTimerMap::value_type(inTimerName,Timer())).first;

	return it->second.GetTotalMiliSeconds();
}

Timer& TimersRegistry::GetTimer(const wstring& inTimerName)
{
	WStringToTimerMap::iterator it = mTimers.find(inTimerName);
	if(it == mTimers.end())
		it = mTimers.insert(WStringToTimerMap::value_type(inTimerName,Timer())).first;

	return it->second;
}

void TimersRegistry::ReleaseAll()
{
	mTimers.clear();
}

void TimersRegistry::TraceAll()
{
	WStringToTimerMap::iterator it = mTimers.begin();
	
	TRACE_LOG("Start Tracing Timers");
	for(; it != mTimers.end(); ++it)
	{
		long hours = (long)it->second.GetTotalMiliSeconds()/3600000;
		long minutes = ((long)it->second.GetTotalMiliSeconds()%3600000) / 60000;
		long seconds = ((long)it->second.GetTotalMiliSeconds()%60000) / 1000;
		long miliseconds = (long)it->second.GetTotalMiliSeconds()%1000;
		TRACE_LOG5("Tracing Timer %s. total time [h:m:s:ms] = %ld:%ld:%ld:%ld",
						it->first.c_str(),hours,minutes,seconds,miliseconds);
	}
	TRACE_LOG("End Tracing Timers");
}

void TimersRegistry::TraceAndReleaseAll()
{
	TraceAll();
	ReleaseAll();
}
