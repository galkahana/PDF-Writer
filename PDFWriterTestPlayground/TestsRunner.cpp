/*
   Source File : TestsRunner.cpp


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
#include "TestsRunner.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

TestsRunner::TestsRunner(void)
{
}

TestsRunner::~TestsRunner(void)
{
	DeleteTests();
}

void TestsRunner::DeleteTests()
{
	StringToStringAndTestUnitListMap::iterator it = mTests.begin();
	for(;it!= mTests.end();++it)
	{
		StringAndTestUnitList::iterator itList = it->second.begin();
		for(; itList != it->second.end();++itList)
			delete(itList->second);
	}
	mTests.clear();
}

EStatusCode TestsRunner::RunAll(const TestConfiguration& inTestConfiguration)
{
	StringAndTestUnitList testsList;
	StringToTestUnitMap::iterator it = mTestsByName.begin();
	for(;it != mTestsByName.end();++it)
		testsList.push_back(StringAndTestUnit(it->first,it->second));
	return RunTestsInList(inTestConfiguration,testsList);
}

EStatusCode TestsRunner::RunTestsInList(const TestConfiguration& inTestConfiguration,const StringAndTestUnitList& inTests)
{
	EStatusCode testsStatus;

	if(inTests.size() == 0)
	{
		cout<<"No tests to run\n";
		testsStatus = PDFHummus::eSuccess;
	}
	else if(inTests.size() == 1)
	{
		testsStatus = RunSingleTest(inTestConfiguration,inTests.front().first,inTests.front().second);
	}
	else
	{
		unsigned long failedCount = 0,succeededCount = 0;
		StringAndTestUnitList::const_iterator it = inTests.begin();
		EStatusCode testStatus;
		testsStatus = PDFHummus::eSuccess;

		cout<<"Start tests run\n";
		for(;it!= inTests.end();++it)
		{
			testStatus = RunSingleTest(inTestConfiguration,it->first,it->second);
			if(PDFHummus::eFailure == testStatus)
			{
				++failedCount;
				testsStatus = PDFHummus::eFailure;
			}
			else
			{
				++succeededCount;
			}
		}
		cout<<"--------------------------------------------------------------\n";
		cout<<"Test run ended: "<<succeededCount<<" tests succeeded, "<<failedCount<<" tests failed.\n";
	}	
	return testsStatus;
}

EStatusCode TestsRunner::RunSingleTest(const TestConfiguration& inTestConfiguration,const string& inTestName,ITestUnit* inTest)
{
	EStatusCode testStatus;
	
	cout<<"Running Test "<<inTestName<<"\n";
	testStatus = inTest->Run(inTestConfiguration);
	if(PDFHummus::eFailure == testStatus)
		cout<<"Test "<<inTestName<<" Failed\n\n";
	else
		cout<<"Test "<<inTestName<<" Succeeded\n\n";
	return testStatus;
}

void TestsRunner::AddTest(const std::string& inTestLabel,const std::string& inCategory,ITestUnit* inTest)
{
	StringToStringAndTestUnitListMap::iterator it = mTests.find(inCategory);
	
	if(it == mTests.end())
		it = mTests.insert(StringToStringAndTestUnitListMap::value_type(inCategory,StringAndTestUnitList())).first;
	it->second.push_back(StringAndTestUnit(inTestLabel,inTest));
	mTestsByName.insert(StringToTestUnitMap::value_type(inTestLabel,inTest));
}

static const string scGeneral= "General";
void TestsRunner::AddTest(const string& inTestLabel,ITestUnit* inTest)
{
	AddTest(inTestLabel,scGeneral,inTest);
}

EStatusCode TestsRunner::RunTest(const TestConfiguration& inTestConfiguration,const string& inTestLabel)
{
	StringToTestUnitMap::iterator it = mTestsByName.find(inTestLabel);

	if(it == mTestsByName.end())
	{
		cout<<"Test not found\n";
		return PDFHummus::eSuccess;
	}
	else
		return RunSingleTest(inTestConfiguration,it->first,it->second);
}

EStatusCode TestsRunner::RunTests(const TestConfiguration& inTestConfiguration,const StringList& inTestsLabels)
{
	StringAndTestUnitList testsList;
	StringList::const_iterator it = inTestsLabels.begin();
	for(; it != inTestsLabels.end(); ++it)
	{
		StringToTestUnitMap::iterator itMap = mTestsByName.find(*it);
		if(itMap != mTestsByName.end())
			testsList.push_back(StringAndTestUnit(itMap->first,itMap->second));
		else
			cout<<"Test "<<*it<<" not found\n";
	}
	return RunTestsInList(inTestConfiguration,testsList);
}

EStatusCode TestsRunner::RunCategory(const TestConfiguration& inTestConfiguration,const string& inCategory)
{
	StringToStringAndTestUnitListMap::iterator it = mTests.find(inCategory);

	if(it == mTests.end())
	{
		cout<<"Category "<<inCategory<<"not found\n";
		return PDFHummus::eSuccess;
	}
	else
		return RunTestsInList(inTestConfiguration,it->second);
}

EStatusCode TestsRunner::RunCategories(const TestConfiguration& inTestConfiguration,const StringList& inCategories)
{
	StringAndTestUnitList testsList;
	StringList::const_iterator it = inCategories.begin();

	for(; it != inCategories.end(); ++it)
	{
		StringToStringAndTestUnitListMap::iterator itMap = mTests.find(*it);
		if(itMap != mTests.end())
			testsList.insert(testsList.end(),itMap->second.begin(),itMap->second.end());
		else
			cout<<"Category "<<*it<<" not found\n";
	}

	return RunTestsInList(inTestConfiguration,testsList);
}

EStatusCode TestsRunner::RunCategories(const TestConfiguration& inTestConfiguration,const StringList& inCategories, const StringSet& inTestsToExclude)
{
	StringAndTestUnitList testsList;
	StringList::const_iterator it = inCategories.begin();

	for(; it != inCategories.end(); ++it)
	{
		StringToStringAndTestUnitListMap::iterator itMap = mTests.find(*it);
		if(itMap != mTests.end())
		{
			StringAndTestUnitList::iterator itCategoryTests = itMap->second.begin();
			for(;itCategoryTests != itMap->second.end();++itCategoryTests)
				if(inTestsToExclude.find(itCategoryTests->first) == inTestsToExclude.end())
					testsList.push_back(StringAndTestUnit(itCategoryTests->first,itCategoryTests->second));
		}
		else
			cout<<"Category "<<*it<<" not found\n";
	}

	return RunTestsInList(inTestConfiguration,testsList);
}


EStatusCode TestsRunner::RunExcludeCategories(const TestConfiguration& inTestConfiguration,const StringSet& inCategories)
{
	StringAndTestUnitList testsList;
	StringToStringAndTestUnitListMap::iterator it = mTests.begin();

	for(; it != mTests.end(); ++it)
		if(inCategories.find(it->first) == inCategories.end())
			testsList.insert(testsList.end(),it->second.begin(),it->second.end());
	return RunTestsInList(inTestConfiguration,testsList);
}

EStatusCode TestsRunner::RunExcludeTests(const TestConfiguration& inTestConfiguration,const StringSet& inTests)
{
	StringAndTestUnitList testsList;
	StringToTestUnitMap::iterator it = mTestsByName.begin();

	for(; it != mTestsByName.end();++it)
		if(inTests.find(it->first) == inTests.end())
			testsList.push_back(StringAndTestUnit(it->first,it->second));

	return RunTestsInList(inTestConfiguration,testsList);
}