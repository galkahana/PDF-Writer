#include "BoxingBaseTest.h"
#include "TestsRunner.h"
#include "BoxingBase.h"

#include <iostream>

using namespace std;

BoxingBaseTest::BoxingBaseTest(void)
{
}

BoxingBaseTest::~BoxingBaseTest(void)
{
}

EStatusCode BoxingBaseTest::Run()
{
	EStatusCode status = RunIntTest();

	return status;
}

EStatusCode BoxingBaseTest::RunIntTest()
{
	EStatusCode status = eSuccess;

		
	// Assignment
	Int a = 3;
	a = 2;
	if((int)a != 2)
	{
		status = eFailure;
		wcout<<"Assignment failed\n";
	}

	// Initialization
	Int b(2);
	if((int)b != 2)
	{
		status = eFailure;
		wcout<<"Initialization failed\n";
	}


	// From string
	Int c("2");
	if((int)c != 2)
	{
		status = eFailure;
		wcout<<"Initialization from string failed\n";
	}

	// From wide string
	Int d(L"2");
	if((int)d != 2)
	{
		status = eFailure;
		wcout<<"Initialization from wide string failed\n";
	}

	// string write
	Int e(2);
	if(e.ToString() != "2")
	{
		status = eFailure;
		wcout<<"Write to string failed\n";
	}

	// wide string write
	Int f(2);
	if(f.ToWString() != L"2")
	{
		status = eFailure;
		wcout<<"Write to wide string failed\n";
	}


	return status;
}

ADD_CETEGORIZED_TEST(BoxingBaseTest,"Patterns")