/*
   Source File : TimerTest.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
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

ADD_CATEGORIZED_TEST(TimerTest,"Basics")
