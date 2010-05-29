#pragma once

#include <string>

using namespace std;

class PDFDate
{
public:
	// enumerator describing the relation to UTC
	enum EUTCRelation
	{
		eEarlier,
		eLater,
		eSame,
		eUndefined
	};


	PDFDate(void); // sets all values to NULL
	~PDFDate(void);

	void SetTime(int inYear,
				 int inMonth = -1,
				 int inDay = -1,
				 int inHour = -1,
				 int inMinute = -1,
				 int inSecond = -1,
				 EUTCRelation inUTC = eUndefined,
				 int inHourFromUTC = -1,
				 int inMinuteFromUTC = -1);

	bool IsNull(); // null is signified by having -1 as the year.
	string ToString(); // writes only non-null values

	// set PDF Date to the current time
	void SetToCurrentTime();

	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int Second;
	EUTCRelation UTC;
	int HourFromUTC;
	int MinuteFromUTC;

};
