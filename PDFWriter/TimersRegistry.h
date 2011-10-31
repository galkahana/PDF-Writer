/*
   Source File : TimersRegistry.h


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
#pragma once

#include "Timer.h"

#include <string>
#include <map>

using namespace std;

typedef map<string,Timer> StringToTimerMap;

class TimersRegistry
{
public:
	TimersRegistry(void);
	~TimersRegistry(void);

	void StartMeasure(const string& inTimerName);
	void StopMeasureAndAccumulate(const string& inTimerName);

	double GetTotalMiliSeconds(const string& inTimerName);

	Timer& GetTimer(const string& inTimerName);

	void ReleaseAll();
	void TraceAll();
	void TraceAndReleaseAll();

private:
	StringToTimerMap mTimers;
};
