#include "UppercaseSequance.h"
#include "Trace.h"

UppercaseSequance::UppercaseSequance(void)
{
	mSequanceString = "AAAAAA";
}

UppercaseSequance::~UppercaseSequance(void)
{
	
}

const string& UppercaseSequance::GetNextValue()
{
	int i = 5;
	
	while(true)
	{
		if(mSequanceString.at(i) < 'Z')
		{
			++mSequanceString.at(i);
			break;
		}
		else
		{
			if(i == 0)
			{
				TRACE_LOG("UppercaseSequance::GetNextValue, oops, reached overflow");
				break;
			}
			mSequanceString.at(i) = 'A';
			--i;
		}
		
	}
	return mSequanceString;
}