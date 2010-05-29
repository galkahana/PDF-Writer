#pragma once

#include "EStatusCode.h"
#include "ITestUnit.h"
#include "Singleton.h"

#include <string>
#include <list>
#include <utility>
#include <map>

using namespace std;

class ITestUnit;

typedef pair<wstring,ITestUnit*> WStringAndTestUnit;
typedef map<wstring,ITestUnit*> WStringToTestUnitMap;
typedef list<WStringAndTestUnit> WStringAndTestUnitList;
typedef map<wstring,WStringAndTestUnitList> WStringToWStringAndTestUnitListMap;

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

	void AddTest(const wstring& inTestLabel,ITestUnit* inTest);
	void AddTest(const std::wstring& inTestLabel,const std::wstring& inCategory,ITestUnit* inTest);

private:
	WStringToWStringAndTestUnitListMap mTests;
	WStringToTestUnitMap mTestsByName;

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