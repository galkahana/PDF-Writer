// PDFWriterTestPlayground.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Singleton.h"
#include "TestsRunner.h"

#include <iostream>

static void PrintUsage()
{
	wcout<<"Usage:\n"<<
		"PDFWRiterTestPlayground [-c category1 category2...] [-t test1 test2...]\n\n"<<
		"PDFWRiterTestPlayground runs tests.\n"<<
		"The non parameterized version runs all tests.\n\n"<<
		"Use -c to run only a group of tests assigned to specific categories. after -c name the cateogries. for example:\n"<<
		"	PDFWriterTestPlayGround -c PDF ObjectContext\n"<<
		"will run only tests in the PDF or ObjectContext categories.\n\n"<<
		"Use -t to run only a group of tests named by the next parameters. after -t name the tests. for example:\n"<<
		"	PDFWriterTestPlayGround -t PDFDateTest PDFTextStingTest\n"<<
		"will run only the PDFDateTest and PDFTextStringTest.\n";
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(3 <= argc)
	{
		if(wcscmp(argv[1],L"-c") == 0) // categories testing
		{
			WStringList categories;
			for(int i=2;i<argc;++i)
				categories.push_back(argv[i]);
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

