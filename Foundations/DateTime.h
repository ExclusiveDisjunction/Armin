#pragma once

#include <iostream>
#include <chrono>
#include <Windows.h>
#include "Str.h"
#include "AString.h"

namespace std::chrono
{
	template <class _Rep, class _Period>
	class duration;
}

enum class DateStringFormat
{
	LongDate = 0,
	ShortDate = 1,
	Time = 2,
	Date = 3,
	Duration = 4
};

class DateTime
{
private:
	std::chrono::duration<double> _Dur;

public:
	static DateTime Today();
	static DateTime Now();

	DateTime();
	DateTime(int Month, int Day, int Year);
	DateTime(int Hour, int Minute, int Second, int Millisecond);
	DateTime(int Month, int Day, int Year, int Hour, int Minute, int Second, int Millisecond);
	DateTime(std::chrono::duration<double> Tm);

	bool HasValue = true, HasDay = true, HasTime = true;

	int Year() const;
	int Year(std::chrono::duration<double>& Remain) const;
	int Month() const;
	int Month(std::chrono::duration<double>& Remain) const;
	int Day() const;
	int Day(std::chrono::duration<double>& Remain) const;
	int Hour() const;
	int Hour(std::chrono::duration<double>& Remain) const;
	int Minute() const;
	int Minute(std::chrono::duration<double>& Remain) const;
	int Second() const;
	int Second(std::chrono::duration<double>& Remain) const;
	int Millisecond() const;
	int Millisecond(std::chrono::duration<double>& Remain) const;

	DateTime DayParts() const;
	DateTime TimeParts() const;

	/**
	* Converts this DateTime to the following format: MM/DD/YYYY HH:MM:SS [AM/PM]
	*/
	String ToString(DateStringFormat Format) const;
	AString ToStringA(DateStringFormat Format) const;
	/**
	* Converts this DateTime to a BackEnd String (BString). This puts it in the format MM:DD:YY
	*/
	String ToBackString() const;
	AString ToBackStringA() const;
	static DateTime FromBackString(String Value);
	static DateTime FromBackStringA(AString Value);

	friend bool operator==(const DateTime& One, const DateTime& Two);
	friend bool operator!=(const DateTime& One, const DateTime& Two);
	bool operator<(const DateTime& Two) const;
	bool operator>(const DateTime& Two) const;
	std::partial_ordering operator<=>(const DateTime& Two) const;

	friend DateTime operator-(const DateTime& One, const DateTime& Two);
	friend DateTime operator+(const DateTime& One, const DateTime& Two);
	DateTime& operator+=(const DateTime& Other);
	DateTime& operator-=(const DateTime& Other);
};