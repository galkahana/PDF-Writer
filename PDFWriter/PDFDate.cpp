#include "PDFDate.h"
#include "SafeBufferMacrosDefs.h"
#include "Trace.h"

#include <ctime>
#include <math.h>

PDFDate::PDFDate(void)
{
	SetTime(-1);
}

PDFDate::~PDFDate(void)
{
}

void PDFDate::SetTime(	int inYear,
						int inMonth,
						int inDay,
						int inHour,
						int inMinute,
						int inSecond,
						EUTCRelation inUTC,
						int inHourFromUTC,
						int inMinuteFromUTC)
{
	Year = inYear;
	Month = inMonth;
	Day = inDay;
	Hour = inHour;
	Minute = inMinute;
	Second = inSecond;
	UTC = inUTC;
	HourFromUTC = inHourFromUTC;
	MinuteFromUTC = inMinuteFromUTC;
}

bool PDFDate::IsNull()
{
	return -1 == Year;	
}

/*
	write a string represetnation of the date. undefined parameters will not
	have values in the string unless later values are defined. in that
	case the PDF specs defaults are used [See section 3.8.3 of PDF Reference, V1.7]

*/

static const string scEmpty = "";
string PDFDate::ToString()
{
	if(IsNull())
		return scEmpty;

	char buffer[24];
	bool wroteSomethingLater = false;

	// UTC info
	if(UTC != PDFDate::eUndefined)
	{
		if(PDFDate::eSame == UTC)
		{
			buffer[17] = '\0';
			buffer[16] = 'Z';
		}
		else
		{
			buffer[23] = '\0';
			buffer[22] = '\'';
			buffer[21] = '0' + ((-1 == MinuteFromUTC ? 0:MinuteFromUTC) % 10);
			buffer[20] = '0' + ((-1 == MinuteFromUTC ? 0:MinuteFromUTC) / 10);
			buffer[19] = '\'';
			buffer[18] = '0' + ((-1 == HourFromUTC ? 0:HourFromUTC) % 10);
			buffer[17] = '0' + ((-1 == HourFromUTC ? 0:HourFromUTC) / 10);
			buffer[16] = (PDFDate::eLater == UTC )? '+':'-';
		}
		wroteSomethingLater = true;
	}

	// Second
	if(Second != -1 || wroteSomethingLater)
	{
		if(!wroteSomethingLater)
		{
			wroteSomethingLater = true;
			buffer[16] = '\0';
		}
		buffer[15] = '0' + ((-1 == Second ? 0:Second) % 10);
		buffer[14] = '0' + ((-1 == Second ? 0:Second) / 10);
	}

	// Minute
	if(Minute != -1 || wroteSomethingLater)
	{
		if(!wroteSomethingLater)
		{
			wroteSomethingLater = true;
			buffer[14] = '\0';
		}
		buffer[13] = '0' + ((-1 == Minute ? 0:Minute) % 10);
		buffer[12] = '0' + ((-1 == Minute ? 0:Minute) / 10);
	}

	// Hour
	if(Hour != -1 || wroteSomethingLater)
	{
		if(!wroteSomethingLater)
		{
			wroteSomethingLater = true;
			buffer[12] = '\0';
		}
		buffer[11] = '0' + ((-1 == Hour ? 0:Hour) % 10);
		buffer[10] = '0' + ((-1 == Hour ? 0:Hour) / 10);
	}

	// Day
	if(Day != -1 || wroteSomethingLater)
	{
		if(!wroteSomethingLater)
		{
			wroteSomethingLater = true;
			buffer[10] = '\0';
		}
		buffer[9] = '0' + ((-1 == Day ? 1:Day) % 10);
		buffer[8] = '0' + ((-1 == Day ? 1:Day) / 10);
	}

	// Month
	if(Month != -1 || wroteSomethingLater)
	{
		if(!wroteSomethingLater)
		{
			wroteSomethingLater = true;
			buffer[8] = '\0';
		}
		buffer[7] = '0' + ((-1 == Month ? 1:Month) % 10);
		buffer[6] = '0' + ((-1 == Month ? 1:Month) / 10);
	}	

	// Year [bug 10K waiting to happen!!!!111]
	if(!wroteSomethingLater)
		buffer[6] = '\0';
	buffer[5] = '0' + Year % 10;
	buffer[4] = '0' + (Year/10) % 10;
	buffer[3] = '0' + (Year/100) % 10;
	buffer[2] = '0' + (Year/1000) % 10;
	buffer[1] = ':';
	buffer[0] = 'D';

	return string(buffer);
}

void PDFDate::SetToCurrentTime()
{
	time_t currentTime;
	tm structuredLocalTime;
	long timeZoneSecondsDifference;

	time(&currentTime);
	SAFE_LOCAL_TIME(structuredLocalTime,currentTime);

	Year = structuredLocalTime.tm_year + 1900;
	Month = structuredLocalTime.tm_mon + 1;
	Day = structuredLocalTime.tm_mday;
	Hour = structuredLocalTime.tm_hour;
	Minute = structuredLocalTime.tm_min;
	Second = structuredLocalTime.tm_sec;

	// if unsuccesful don't provide UTC info
	if(_get_timezone(&timeZoneSecondsDifference) == 0)
	{
		if(0 == timeZoneSecondsDifference)
		{
			UTC = eSame;
		}
		else
		{
			UTC = timeZoneSecondsDifference > 0 ? eLater:eEarlier;
			HourFromUTC = abs(timeZoneSecondsDifference) / 3600;
			MinuteFromUTC = (abs(timeZoneSecondsDifference) - (abs(timeZoneSecondsDifference) / 3600)*3600) / 60;
		}
	}
	else
	{
		TRACE_LOG("PDFDate::SetToCurrentTime, Couldn't get UTC.");
	}
}