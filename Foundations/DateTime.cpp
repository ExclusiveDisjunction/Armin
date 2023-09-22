#include "DateTime.h"

#include "Container.h"
#include "Str.h"

#include <chrono>

using namespace std;
using namespace std::chrono;

DateTime::DateTime() : _Dur(0)
{
}
DateTime::DateTime(int Month, int Day, int Year)
{
	if (!(Month >= 1 && Month <= 12))
		Month = 1;
	if (!(Day >= 1 && Day <= 31))
		Day = 1;

	Month--;
	Day--;

	years Y(Year);
	months M(Month);
	days D(Day);

	_Dur = Y + M + D;
}
DateTime::DateTime(int Hour, int Minute, int Second, int Millisecond)
{
	if (Hour > 23 || Hour < 0)
		Hour = 1;
	if (Minute > 59 || Minute < 0)
		Minute = 1;
	if (Second > 59 || Minute < 0)
		Second = 1;

	hours H(Hour);
	minutes Mi = minutes(Minute);
	seconds S(Second);
	milliseconds Ms(Millisecond);

	_Dur = H + Mi + S + Ms;
}
DateTime::DateTime(int Month, int Day, int Year, int Hour, int Minute, int Second, int Millisecond)
{
	if (!(Month >= 1 && Month <= 12))
		Month = 1;
	if (!(Day >= 1 && Day <= 31))
		Day= 1;
	if (Hour > 23 || Hour < 0)
		Hour = 1;
	if (Minute > 59 || Minute < 0)
		Minute = 1;
	if (Second > 59 || Minute < 0)
		Second = 1;

	Month--;
	Day--;
	Year -= 1970;

	years Y(Year);
	months M(Month);
	days D(Day);
	hours H(Hour);
	minutes Mi = minutes(Minute);
	seconds S(Second);
	milliseconds Ms(Millisecond);

	_Dur = Y + M + D + H + Mi + S + Ms;
}
DateTime::DateTime(duration<double> Dur)
{
	_Dur = Dur;
}

DateTime DateTime::Today()
{
	DateTime Return;

	auto Point = system_clock::now();
	auto Zone = current_zone();
	auto Zoned = zoned_time(current_zone(), Point);

	Return._Dur = Zoned.get_local_time().time_since_epoch();
	Return = Return.DayParts();

	return Return;
}
DateTime DateTime::Now()
{
	DateTime Return;

	auto Point = system_clock::now();
	auto Zoned = zoned_time(current_zone(), Point);

	Return._Dur = Zoned.get_local_time().time_since_epoch();

	return Return;
}

int DateTime::Year() const
{
	duration<double> This = this->_Dur;
	return Year(This) + 1970;
}
int DateTime::Year(duration<double>& Remain) const
{
	years Return = duration_cast<years>(Remain);
	Remain -= Return;

	return Return.count();
}
int DateTime::Month() const
{
	duration<double> This = this->_Dur;
	Year(This);
	return Month(This) + 1;
}
int DateTime::Month(std::chrono::duration<double>& Remain) const 
{
	months Return = duration_cast<months>(Remain);
	Remain -= Return;

	return Return.count();
}
int DateTime::Day() const
{
	duration<double> This = this->_Dur;
	Month(This);
	return Day(This) + 1;
}
int DateTime::Day(std::chrono::duration<double>& Remain) const
{
	days Return = duration_cast<days>(Remain);
	Remain -= Return;

	return Return.count();
}
int DateTime::Hour() const 
{
	duration<double> This = this->_Dur;
	Day(This);
	return Hour(This);
}
int DateTime::Hour(std::chrono::duration<double>& Remain) const 
{
	hours Return = duration_cast<hours>(Remain);
	Remain -= Return;

	return Return.count();
}
int DateTime::Minute() const
{
	duration<double> This = this->_Dur;
	Hour(This);
	return Minute(This);
}
int DateTime::Minute(std::chrono::duration<double>& Remain) const
{
	minutes Return = duration_cast<minutes>(Remain);
	Remain -= Return;

	return Return.count();
}
int DateTime::Second() const
{
	duration<double> This = this->_Dur;
	Minute(This);
	return Second(This);
}
int DateTime::Second(std::chrono::duration<double>& Remain) const
{
	seconds Return = duration_cast<seconds>(Remain);
	Remain -= Return;

	return static_cast<int>(Return.count());
}
int DateTime::Millisecond() const
{
	duration<double> This = this->_Dur;
	Second(This);
	return Millisecond(This);
}
int DateTime::Millisecond(duration<double>& Remain) const
{
	milliseconds Return = duration_cast<milliseconds>(Remain);
	Remain -= Return;

	return static_cast<int>(Return.count());
}

DateTime DateTime::DayParts() const
{
	DateTime New;

	duration<double>& Dur = New._Dur;
	Dur = _Dur;
	years Y = duration_cast<years>(Dur);
	Dur -= Y;
	months M = duration_cast<months>(Dur);
	Dur -= M;
	days D = duration_cast<days>(Dur);
	Dur -= D;

	Dur = Y + M + D; //Once Y:M:D is extracted, Dur is set to the sum of these three parts, excluding H:M:S:MI
	return New;
}
DateTime DateTime::TimeParts() const
{
	DateTime New;

	duration<double>& Dur = New._Dur;
	Dur = _Dur;
	years Y = duration_cast<years>(Dur);
	Dur -= Y;
	months M = duration_cast<months>(Dur);
	Dur -= M;
	days D = duration_cast<days>(Dur);
	Dur -= D;

	return New; //All that is left is simply H:M:S:MS. No further action must take place.
}

String DateTime::ToString(DateStringFormat Format) const
{
	int Year = this->Year(), Month = this->Month(), Day = this->Day(), Hour = this->Hour(), Minute = this->Minute(), Second = this->Second(), Millisecond = this->Millisecond();

	String Return;

	switch (Format)
	{
	case DateStringFormat::Date:
	{
		if (Day != 0 && Year != 0 && Month != 0)
			Return = String(Month) + String(L"/") + String(Day) + String(L"/") + String(Year);
		else
			Return = L"No Date";
		break;
	}
	case DateStringFormat::Duration:
	{
		if (Year == 0 && Month == 0 && Day == 0 && Hour == 0 && Minute == 0 && Second == 0)
			return L"No Time";

		String Y = Year == 0 ? String() : String(Year) +  (Year == 1 ? L"Year" : L"Years");
		String M = Month == 0 ? String() : String(Month) + (Month == 1 ? L"Month" : L"Months");
		String D = Day == 0 ? String() : String(Day) + (Day == 1 ? L"Day" : L"Days");
		String H = Hour == 0 ? String() : String(Hour) + (Hour == 1 ? L"Hour" : L"Hours");
		String Mi = Minute == 0 ? String() : String(Minute) + (Minute == 1 ? L"Minute" : L"Minutes");
		String S = Second == 0 ? String() : String(Second) + (Second == 1 ? L"Second" : L"Seconds");

		Return = Y + (Y == String() ? String() : L", ") + String(M) + String(M == String() ? String() : L", ") + String(D) + (D == String() ? String() : L", ") + String(H) + (H == String() ? String() : L", ") + Mi + (Mi == String() ? String() : L", ") + String(S) + (S == String() ? String() : L", ");
		break;
	}
	case DateStringFormat::LongDate:
	{
		if (Month != 0 && Year != 0 && Day != 0)
		{
			switch (Month)
			{
			case 1:
				Return = L"January";
				break;
			case 2:
				Return = L"February";
				break;
			case 3:
				Return = L"March";
				break;
			case 4:
				Return = "April";
				break;
			case 5:
				Return = L"May";
				break;
			case 6:
				Return = L"June";
				break;
			case 7:
				Return = L"July";
				break;
			case 8:
				Return = L"August";
				break;
			case 9:
				Return = L"September";
				break;
			case 10:
				Return = L"October";
				break;
			case 11:
				Return = L"November";
				break;
			case 12:
				Return = L"December";
				break;
			}

			Return += (L" ") + String(Day);
			switch (Day)
			{
			case 1:
			case 21:
			case 31:
				Return += L"st";
				break;

			case 2:
			case 22:
				Return += L"nd";
				break;

			case 3:
			case 23:
				Return += L"rd";
				break;

			default:
				Return += L"th";
				break;
			}

			Return += String(L", ") + String(Year);
		}

		if (Hour != 0 && Minute != 0)
			Return += (Year == 0 || Month == 0 || Day == 0 ? L"" : String(L" at ")) + String(Hour > 12 ? Hour - 12 : Hour) + L":" + String(Minute) + String(Hour < 12 ? L"AM" : L"PM");
		break;
	}
	case DateStringFormat::ShortDate:
	{
		if (Day != 0 && Year != 0 && Month != 0)
			Return = String(Month) + String(L"/") + String(Day) + String(L"/") + String(Year);

		if (Hour != 0 && Minute != 0)
			Return += (Year == 0 || Month == 0 || Day == 0 ? L"" : String(L" at ")) + String(Hour > 12 ? Hour - 12 : Hour) + L":" + String(Minute) + String(Hour < 12 ? L"AM" : L"PM");
		break;
	}
	case DateStringFormat::Time:
	{
		if (Hour != 0 && Minute != 0)
			Return += (Year == 0 || Month == 0 || Day == 0 ? L"" : String(L" at ")) + String(Hour > 12 ? Hour - 12 : Hour) + L":" + String(Minute) + String(Hour < 12 ? L"AM" : L"PM");
		else
			Return = L"No Time";
		break;
	}
	}

	return Return;
}
AString DateTime::ToStringA(DateStringFormat Format) const
{
	return (AString)ToString(Format);
}

String DateTime::ToBackString() const
{
	return _Dur.count();
}
AString DateTime::ToBackStringA() const
{
	return _Dur.count();
}
DateTime DateTime::FromBackString(String Value)
{
	duration<double> Raw(Value.ToDouble());

	return DateTime(Raw);
}
DateTime DateTime::FromBackStringA(AString Value)
{
	duration<double> Raw(Value.ToDouble());

	return DateTime(Raw);
}

bool operator==(const DateTime& One, const DateTime& Two)
{
	return One._Dur == Two._Dur;
}
bool operator!=(const DateTime& One, const DateTime& Two)
{
    return !(One == Two);
}

bool DateTime::operator<(const DateTime& Two) const
{
	return _Dur < Two._Dur;
}
bool DateTime::operator>(const DateTime& Two) const
{
	return _Dur > Two._Dur;
}
std::partial_ordering DateTime::operator<=>(const DateTime& Two) const
{
	return _Dur <=> Two._Dur;
}

DateTime operator-(const DateTime& One, const DateTime& Two)
{
	return DateTime(One._Dur - Two._Dur);
}
DateTime operator+(const DateTime& One, const DateTime& Two)
{
	return DateTime(One._Dur + Two._Dur);
}
DateTime& DateTime::operator+=(const DateTime& Two)
{
	_Dur += Two._Dur;
	return *this;
}
DateTime& DateTime::operator-=(const DateTime& Two)
{
	_Dur -= Two._Dur;
	return *this;
}
