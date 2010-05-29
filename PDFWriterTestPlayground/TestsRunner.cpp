#include "TestsRunner.h"

#include <iostream>

using namespace std;

TestsRunner::TestsRunner(void)
{
}

TestsRunner::~TestsRunner(void)
{
	DeleteTests();
}

void TestsRunner::DeleteTests()
{
	WStringToWStringAndTestUnitListMap::iterator it = mTests.begin();
	for(;it!= mTests.end();++it)
	{
		WStringAndTestUnitList::iterator itList = it->second.begin();
		for(; itList != it->second.end();++itList)
			delete(itList->second);
	}
	mTests.clear();
}

EStatusCode TestsRunner::RunAll()
{
	EStatusCode testsStatus = eSuccess;
	unsigned long failedCount = 0,succeededCount = 0;

	if(mTests.size() == 0)
	{
		wcout<<L"No tests to run\n";
	}
	else
	{
		WStringToWStringAndTestUnitListMap::iterator it = mTests.begin();
		EStatusCode testStatus;

		wcout<<L"Start tests run\n";

		for(;it!= mTests.end();++it)
		{
			WStringAndTestUnitList::iterator itList = it->second.begin();
			for(;itList != it->second.end();++itList)
			{
				wcout<<L"Running Test "<<itList->first<<L"\n";
				testStatus = itList->second->Run();
				if(eFailure == testStatus)
				{
					wcout<<L"Test "<<itList->first<<L" Failed\n\n";
					++failedCount;
					testsStatus = eFailure;
				}
				else
				{
					wcout<<L"Test "<<itList->first<<L" Succeeded\n\n";
					++succeededCount;
				}
			}
		}
		wcout<<L"--------------------------------------------------------------\n";
		wcout<<L"Test run ended: "<<succeededCount<<L" tests succeeded, "<<failedCount<<L" tests failed.\n";

	}
	return testsStatus;
}

void TestsRunner::AddTest(const std::wstring& inTestLabel,const std::wstring& inCategory,ITestUnit* inTest)
{
	WStringToWStringAndTestUnitListMap::iterator it = mTests.find(inCategory);
	
	if(it == mTests.end())
		it = mTests.insert(WStringToWStringAndTestUnitListMap::value_type(inCategory,WStringAndTestUnitList())).first;
	it->second.push_back(WStringAndTestUnit(inTestLabel,inTest));
	mTestsByName.insert(WStringToTestUnitMap::value_type(inTestLabel,inTest));
}

static const wstring scGeneral= L"General";
void TestsRunner::AddTest(const wstring& inTestLabel,ITestUnit* inTest)
{
	AddTest(inTestLabel,scGeneral,inTest);
}

EStatusCode TestsRunner::RunTest(const wstring& inTestLabel)
{
	EStatusCode testStatus = eSuccess;
	WStringToTestUnitMap::iterator it = mTestsByName.find(inTestLabel);
	if(it == mTestsByName.end())
	{
		wcout<<L"Test not found\n";
	}
	else
	{
		wcout<<L"Running Test "<<inTestLabel<<L"\n";
		testStatus = it->second->Run();
		if(eFailure == testStatus)
			wcout<<L"Test "<<inTestLabel<<L" Failed\n\n";
		else
			wcout<<L"Test "<<inTestLabel<<L" Succeeded\n\n";
	}
	return testStatus;
}

EStatusCode TestsRunner::RunTests(const WStringList& inTestsLabels)
{
	EStatusCode testsStatus = eSuccess;
	unsigned long failedCount = 0,succeededCount = 0;

	WStringAndTestUnitList testsList;
	WStringList::const_iterator it = inTestsLabels.begin();
	for(; it != inTestsLabels.end(); ++it)
	{
		WStringToTestUnitMap::iterator itMap = mTestsByName.find(*it);
		if(itMap != mTestsByName.end())
			testsList.push_back(WStringAndTestUnit(itMap->first,itMap->second));
		else
			wcout<<L"Test "<<*it<<" not found\n";
	}

	wcout<<L"Start tests run\n";

	WStringAndTestUnitList::iterator itList = testsList.begin();
	EStatusCode testStatus;
	
	for(;itList != testsList.end();++itList)
	{
		wcout<<L"Running Test "<<itList->first<<L"\n";
		testStatus = itList->second->Run();
		if(eFailure == testStatus)
		{
			wcout<<L"Test "<<itList->first<<L" Failed\n\n";
			++failedCount;
			testsStatus = eFailure;
		}
		else
		{
			wcout<<L"Test "<<itList->first<<L" Succeeded\n\n";
			++succeededCount;
		}
	}
	wcout<<L"--------------------------------------------------------------\n";
	wcout<<L"Test run ended: "<<succeededCount<<L" tests succeeded, "<<failedCount<<L" tests failed.\n";

	return testsStatus;
}

EStatusCode TestsRunner::RunCategory(const wstring& inCategory)
{
	EStatusCode testsStatus = eSuccess;
	EStatusCode testStatus;
	unsigned long failedCount = 0,succeededCount = 0;
	WStringToWStringAndTestUnitListMap::iterator it = mTests.find(inCategory);

	if(it == mTests.end())
	{
		wcout<<L"Category "<<inCategory<<L"not found\n";
	}
	else
	{
		wcout<<L"Start tests run\n";
		WStringAndTestUnitList::iterator itList = it->second.begin();

		for(;itList!= it->second.end();++itList)
		{
			wcout<<L"Running Test "<<itList->first<<L"\n";
			testStatus = itList->second->Run();
			if(eFailure == testStatus)
			{
				wcout<<L"Test "<<itList->first<<L" Failed\n\n";
				++failedCount;
			}
			else
			{
				wcout<<L"Test "<<itList->first<<L" Succeeded\n\n";
				++succeededCount;
			}
		}
		wcout<<L"--------------------------------------------------------------\n";
		wcout<<L"Test run ended: "<<succeededCount<<L" tests succeeded, "<<failedCount<<L" tests failed.\n";
	}	
	return testsStatus;
}

EStatusCode TestsRunner::RunCategories(const WStringList& inCategories)
{
	EStatusCode testsStatus = eSuccess;
	unsigned long failedCount = 0,succeededCount = 0;

	WStringAndTestUnitList testsList;
	WStringList::const_iterator it = inCategories.begin();
	for(; it != inCategories.end(); ++it)
	{
		WStringToWStringAndTestUnitListMap::iterator itMap = mTests.find(*it);
		if(itMap != mTests.end())
		{
			testsList.insert(testsList.end(),itMap->second.begin(),itMap->second.end());
		}
		else
			wcout<<L"Category "<<*it<<" not found\n";
	}

	wcout<<L"Start tests run\n";

	WStringAndTestUnitList::iterator itList = testsList.begin();
	EStatusCode testStatus;
	
	for(;itList != testsList.end();++itList)
	{
		wcout<<L"Running Test "<<itList->first<<L"\n";
		testStatus = itList->second->Run();
		if(eFailure == testStatus)
		{
			wcout<<L"Test "<<itList->first<<L" Failed\n\n";
			++failedCount;
			testsStatus = eFailure;
		}
		else
		{
			wcout<<L"Test "<<itList->first<<L" Succeeded\n\n";
			++succeededCount;
		}
	}
	wcout<<L"--------------------------------------------------------------\n";
	wcout<<L"Test run ended: "<<succeededCount<<L" tests succeeded, "<<failedCount<<L" tests failed.\n";

	return testsStatus;	
}