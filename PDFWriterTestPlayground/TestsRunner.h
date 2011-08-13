/*
   Source File : TestsRunner.h


   Copyright 2011 Gal Kahana HummusPDFWriter

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

#include "EPDFStatusCode.h"
#include "ITestUnit.h"
#include "Singleton.h"

#include <string>
#include <list>
#include <utility>
#include <map>
#include <set>

using namespace std;

class ITestUnit;

typedef pair<string,ITestUnit*> StringAndTestUnit;
typedef map<string,ITestUnit*> StringToTestUnitMap;
typedef list<StringAndTestUnit> StringAndTestUnitList;
typedef map<string,StringAndTestUnitList> StringToStringAndTestUnitListMap;
typedef set<string> StringSet;

typedef list<string> StringList;

class TestsRunner
{
public:
	TestsRunner(void);
	~TestsRunner(void);


	EPDFStatusCode RunAll();
	EPDFStatusCode RunTest(const string& inTestLabel);
	EPDFStatusCode RunTests(const StringList& inTestsLabels);
	EPDFStatusCode RunCategory(const string& inCategory);
	EPDFStatusCode RunCategories(const StringList& inCategories);
	EPDFStatusCode RunCategories(const StringList& inCategories, const StringSet& inTestsToExclude);
	EPDFStatusCode RunExcludeCategories(const StringSet& inCategories);
	EPDFStatusCode RunExcludeTests(const StringSet& inTests);

	void AddTest(const string& inTestLabel,ITestUnit* inTest);
	void AddTest(const std::string& inTestLabel,const std::string& inCategory,ITestUnit* inTest);

private:
	StringToStringAndTestUnitListMap mTests;
	StringToTestUnitMap mTestsByName;

	EPDFStatusCode RunTestsInList(const StringAndTestUnitList& inTests);
	EPDFStatusCode RunSingleTest(const string& inTestName,ITestUnit* inTest);
	void DeleteTests();
};

// make sure to use ADD_TEST or ADD_CATEGORIZED_TEST for the test class in your local CPP code...in order not to get
// multiple definitions of TestRegistrar
#define ADD_TEST(X) \
template <class T> \
class TestRegistrar { \
	public:  \
		TestRegistrar(){ \
			Singleton<TestsRunner>::GetInstance()->AddTest(#X,new T); \
		} \
};  \
static TestRegistrar<X> TestRegistrar;

#define ADD_CATEGORIZED_TEST(X,CATEGORY) \
template <class T> \
class TestRegistrar { \
	public:  \
		TestRegistrar(){ \
		Singleton<TestsRunner>::GetInstance()->AddTest(#X,##CATEGORY,new T); \
		} \
};  \
static TestRegistrar<X> TestRegistrar;