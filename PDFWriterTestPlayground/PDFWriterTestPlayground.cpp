// PDFWriterTestPlayground.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Singleton.h"
#include "TestsRunner.h"

#include <iostream>

static void PrintUsage()
{
	wcout<<"Usage:\n"<<
		"PDFWRiterTestPlayground [-c category1 category2... [-xt test1 test2...]] [-t test1 test2...] [-xc category1 category2...] [-xt test1 test2...]\n\n"<<
		"PDFWRiterTestPlayground runs tests.\n"<<
		"The non parameterized version runs all tests.\n\n"<<
		"Use -c to run only a group of tests assigned to specific categories. after -c name the cateogries. for example:\n"<<
		"	PDFWriterTestPlayGround -c PDF ObjectContext\n"<<
		"will run only tests in the PDF or ObjectContext categories.\n\n"<<
		"Use -t to run only a group of tests named by the next parameters. after -t name the tests. for example:\n"<<
		"	PDFWriterTestPlayGround -t PDFDateTest PDFTextStingTest\n"<<
		"will run only the PDFDateTest and PDFTextStringTest.\n"<<
		"Use -xc to all but the group of tests assigned to specific categories. after -xc name the cateogries. for example:\n"<<
		"	PDFWriterTestPlayGround -xc PDF ObjectContext\n"<<
		"Use -xt to run all but a group of tests named by the next parameters. after -xt name the tests. for example:\n"<<
		"	PDFWriterTestPlayGround -t PDFDateTest PDFTextStingTest\n"<<
		"it is possible to use -xt after -c directive to filter particular tests for the executed categories. for example:\n"<<
		"	PDFWriterTestPlayGround -c PDF -xt TiffImageTest.";
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(3 <= argc)
	{
		if(wcscmp(argv[1],L"-c") == 0) // categories testing
		{
			bool hasExclusions = false;
			WStringList categories;
			int i=2;
			
			for(;i<argc;++i)
			{
				if(wcscmp(argv[i],L"-xt") == 0)
				{
					hasExclusions = true;
					break;
				}
				else
					categories.push_back(argv[i]);
			}

			if(hasExclusions)
			{
				WStringSet excludedTests;
				for(;i<argc;++i)
					excludedTests.insert(argv[i]);
				Singleton<TestsRunner>::GetInstance()->RunCategories(categories,excludedTests);
			}
			else
				Singleton<TestsRunner>::GetInstance()->RunCategories(categories);
			Singleton<TestsRunner>::Reset();
		}
		else if(wcscmp(argv[1],L"-t") == 0) // per test label testing
		{
			WStringList tests;
			for(int i=2;i<argc;++i)
				tests.push_back(argv[i]);
			Singleton<TestsRunner>::GetInstance()->RunTests(tests);
			Singleton<TestsRunner>::Reset();
		}
		else if(wcscmp(argv[1],L"-xc") == 0) // all tests with categories exclusion
		{
			WStringSet categories;
			for(int i=2;i<argc;++i)
				categories.insert(argv[i]);
			Singleton<TestsRunner>::GetInstance()->RunExcludeCategories(categories);
			Singleton<TestsRunner>::Reset();
		}
		else if(wcscmp(argv[1],L"-xt") == 0) // all tests with categories exclusion
		{
			WStringSet tests;
			for(int i=2;i<argc;++i)
				tests.insert(argv[i]);
			Singleton<TestsRunner>::GetInstance()->RunExcludeTests(tests);
			Singleton<TestsRunner>::Reset();
		}
		else
			PrintUsage();
	}
	else if(1 == argc)
	{
		Singleton<TestsRunner>::GetInstance()->RunAll();
		Singleton<TestsRunner>::Reset();
	}
	else
		PrintUsage();

	std::cin.get();

	return 0;
}

