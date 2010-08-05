#include "TimerTest.h"
#include "Trace.h"
#include "TimersRegistry.h"

#include <time.h>


TimerTest::TimerTest(void)
{
}

TimerTest::~TimerTest(void)
{
}

EStatusCode TimerTest::Run()
{
	Singleton<Trace>::GetInstance()->SetLogSettings(L"c:\\PDFLibTests\\timersTrace.txt",true);

	TimersRegistry timersRegistry;

	timersRegistry.StartMeasure(L"TimerA");
	timersRegistry.StartMeasure(L"TimerB");
	Sleep(5);
	timersRegistry.StopMeasureAndAccumulate(L"TimerA");
	timersRegistry.StopMeasureAndAccumulate(L"TimerB");

	timersRegistry.StartMeasure(L"TimerC");
	timersRegistry.StartMeasure(L"TimerB");
	Sleep(5.80);
	timersRegistry.StopMeasureAndAccumulate(L"TimerC");
	timersRegistry.StopMeasureAndAccumulate(L"TimerB");

	timersRegistry.TraceAndReleaseAll();

	Singleton<Trace>::Reset();

	return eSuccess;
}

void TimerTest::Sleep(double inSeconds)
{
	clock_t wait = (clock_t)(inSeconds * CLOCKS_PER_SEC);
	clock_t goal;
	goal = wait + clock();
	while(goal > clock())
		;
}

ADD_CETEGORIZED_TEST(TimerTest,"Basics")