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
#include "FileURL.h"

#include <string.h>

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

	PDFHummus::EStatusCode RunAll(const TestConfiguration& inTestConfiguration);
	PDFHummus::EStatusCode RunTest(const TestConfiguration& inTestConfiguration,const string& inTestLabel);
	PDFHummus::EStatusCode RunTests(const TestConfiguration& inTestConfiguration,const StringList& inTestsLabels);
	PDFHummus::EStatusCode RunCategory(const TestConfiguration& inTestConfiguration,const string& inCategory);
	PDFHummus::EStatusCode RunCategories(const TestConfiguration& inTestConfiguration,const StringList& inCategories);
	PDFHummus::EStatusCode RunCategories(const TestConfiguration& inTestConfiguration,const StringList& inCategories, const StringSet& inTestsToExclude);
	PDFHummus::EStatusCode RunExcludeCategories(const TestConfiguration& inTestConfiguration,const StringSet& inCategories);
	PDFHummus::EStatusCode RunExcludeTests(const TestConfiguration& inTestConfiguration,const StringSet& inTests);

	void AddTest(const string& inTestLabel,ITestUnit* inTest);
	void AddTest(const std::string& inTestLabel,const std::string& inCategory,ITestUnit* inTest);

private:
	StringToStringAndTestUnitListMap mTests;
	StringToTestUnitMap mTestsByName;

	PDFHummus::EStatusCode RunTestsInList(const TestConfiguration& inTestConfiguration,const StringAndTestUnitList& inTests);
	PDFHummus::EStatusCode RunSingleTest(const TestConfiguration& inTestConfiguration,const string& inTestName,ITestUnit* inTest);
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
		Singleton<TestsRunner>::GetInstance()->AddTest(#X,CATEGORY,new T); \
		} \
};  \
static TestRegistrar<X> TestRegistrarInstance;


// Helper macro to get a path string from file URL, according to the current OS
#ifdef WIN32

    #include "WindowsPath.h"
    static string FileURLToLocalPath(const FileURL& inFileURL)
    {
        WindowsPath windowsPath;
        windowsPath.FromFileURL(inFileURL);
        return windowsPath.ToString();
    }

    static FileURL LocalPathToFileURL(const string& inLocalPath)
    {
        WindowsPath windowsPath(inLocalPath);
        return windowsPath.ToFileURL();
    }
#else

    #include "PosixPath.h"
    static string FileURLToLocalPath(const FileURL& inFileURL)
    {
        PosixPath posixPath;
        posixPath.FromFileURL(inFileURL);
        return posixPath.ToString();
    }

    static FileURL LocalPathToFileURL(const string& inLocalPath)
    {
        PosixPath posixPath(inLocalPath);
        return posixPath.ToFileURL();
    }
#endif

static string RelativeURLToLocalPath(const FileURL& inFileURL,const string& inRelativeURL)
{
    return FileURLToLocalPath(FileURL(inRelativeURL).InterpretFrom(inFileURL));
}
