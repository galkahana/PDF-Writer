/*
   Source File : TestsRunner.h


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

#include "EStatusCode.h"
#include "ITestUnit.h"
#include "Singleton.h"

#include <string>
#include <list>
#include <utility>
#include <map>
#include <set>

using namespace std;

class ITestUnit;

typedef pair<wstring,ITestUnit*> WStringAndTestUnit;
typedef map<wstring,ITestUnit*> WStringToTestUnitMap;
typedef list<WStringAndTestUnit> WStringAndTestUnitList;
typedef map<wstring,WStringAndTestUnitList> WStringToWStringAndTestUnitListMap;
typedef set<wstring> WStringSet;

typedef list<wstring> WStringList;

class TestsRunner
{
public:
	TestsRunner(void);
	~TestsRunner(void);


	EStatusCode RunAll();
	EStatusCode RunTest(const wstring& inTestLabel);
	EStatusCode RunTests(const WStringList& inTestsLabels);
	EStatusCode RunCategory(const wstring& inCategory);
	EStatusCode RunCategories(const WStringList& inCategories);
	EStatusCode RunCategories(const WStringList& inCategories, const WStringSet& inTestsToExclude);
	EStatusCode RunExcludeCategories(const WStringSet& inCategories);
	EStatusCode RunExcludeTests(const WStringSet& inTests);

	void AddTest(const wstring& inTestLabel,ITestUnit* inTest);
	void AddTest(const std::wstring& inTestLabel,const std::wstring& inCategory,ITestUnit* inTest);

private:
	WStringToWStringAndTestUnitListMap mTests;
	WStringToTestUnitMap mTestsByName;

	EStatusCode RunTestsInList(const WStringAndTestUnitList& inTests);
	EStatusCode RunSingleTest(const wstring& inTestName,ITestUnit* inTest);
	void DeleteTests();
};

// make sure to use ADD_TEST or ADD_CETEGORIZED_TEST for the test class in your local CPP code...in order not to get
// multiple definitions of TestRegistrar
#define ADD_TEST(X) \
template <class T> \
class TestRegistrar { \
	public:  \
		TestRegistrar(){ \
			Singleton<TestsRunner>::GetInstance()->AddTest(L#X,new T); \
		} \
};  \
static TestRegistrar<X> TestRegistrar;

#define ADD_CETEGORIZED_TEST(X,CATEGORY) \
template <class T> \
class TestRegistrar { \
	public:  \
		TestRegistrar(){ \
		Singleton<TestsRunner>::GetInstance()->AddTest(L#X,L##CATEGORY,new T); \
		} \
};  \
static TestRegistrar<X> TestRegistrar;