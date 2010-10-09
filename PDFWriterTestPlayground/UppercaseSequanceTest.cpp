#include "UppercaseSequanceTest.h"
#include "UppercaseSequance.h"
#include <iostream>

using namespace std;

UppercaseSequanceTest::UppercaseSequanceTest(void)
{
}

UppercaseSequanceTest::~UppercaseSequanceTest(void)
{
}

EStatusCode UppercaseSequanceTest::Run()
{
	EStatusCode status = eSuccess;
	UppercaseSequance sequance;

	do
	{
		string value = sequance.GetNextValue();
		if(value != "AAAAAB")
		{
			wcout<<"Failed in single increase. value is  "<<value.c_str()<<". should be AAAAAB\n";
			status = eFailure;
			break;
		}
		else
		{
			wcout<<"Success in single increase\n";
		}
	
		for(int i=0;i<25;++i)
			value = sequance.GetNextValue();
		if(value != "AAAABA")
		{
			wcout<<"Failed in 24 increase. value is  "<<value.c_str()<<". should be AAAABA\n";
			status = eFailure;
			break;
		}
		else
		{
			wcout<<"Success in 24 increase\n";
		}

		for(int j=0;j < 26*25; ++j)
			value = sequance.GetNextValue();
		if(value != "AAABAA")
		{
			wcout<<"Failed in 26*25 increase. value is  "<<value.c_str()<<". should be AAABAA\n";
			status = eFailure;
			break;
		}
		else
		{
			wcout<<"Success in 26*25 increase\n";
		}
	
	}while(false);

	return status;
}


ADD_CETEGORIZED_TEST(UppercaseSequanceTest,"ObjectContext")