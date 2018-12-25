/*************************************************************************
*  Sleep Motivator
* ------------------------------------------------------------------------
*  Copyright (c) 2018 Vladimir Sviridov <v.shtille@gmail.com>
*
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
*
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
*
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
*
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
*
*  3. This notice may not be removed or altered from any source
*     distribution.
*
**************************************************************************/

#include "time_info.h"

#include <ctime>

namespace sm {

	static int kDaysInYear = 365;

	static inline bool IsLeapYear(int year)
	{
		return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	}
	static inline int GetNumberOfDaysInYear(int year)
	{
		if (IsLeapYear(year))
			return 366;
		else
			return 365;
	}

	void TimeInfo::MakeCurrent()
	{
		// Get local time information
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		minute = time_info.tm_min;
		hour = time_info.tm_hour;
		year_day = time_info.tm_yday;
		year = time_info.tm_year;
	}
	void TimeInfo::Make(int hours, int minutes)
	{
		// Get local time information
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		minute = minutes;
		hour = hours;
		year_day = time_info.tm_yday;
		year = time_info.tm_year;
	}
	void TimeInfo::MakeWakeUp(int hours, int minutes)
	{
		// Get local time information
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		minute = minutes;
		hour = hours;
		year_day = time_info.tm_yday;
		year = time_info.tm_year;

		if (time_info.tm_hour > hours)
			PlusDays(1);
	}
	void TimeInfo::MinusMinutes(int minutes)
	{
		minute -= minutes;
		if (minute < 0)
		{
			minute += 60;
			--hour;
			if (hour < 0)
			{
				hour += 24;
				--year_day;
				if (year_day < 0)
				{
					year_day += kDaysInYear;
					--year;
				}
			}
		}
	}
	void TimeInfo::PlusMinutes(int minutes)
	{
		minute += minutes;
		if (minute > 59)
		{
			minute -= 60;
			++hour;
			if (hour > 23)
			{
				hour -= 24;
				++year_day;
				if (year_day >= kDaysInYear)
				{
					year_day -= kDaysInYear;
					++year;
				}
			}
		}
	}
	void TimeInfo::MinusHours(int hours)
	{
		hour -= hours;
		if (hour < 0)
		{
			hour += 24;
			--year_day;
			if (year_day < 0)
			{
				year_day += kDaysInYear;
				--year;
			}
		}
	}
	void TimeInfo::MinusDays(int days)
	{
		year_day -= days;
		if (year_day < 0)
		{
			year_day += kDaysInYear;
			--year;
		}
	}
	void TimeInfo::PlusDays(int days)
	{
		year_day += days;
		if (year_day >= kDaysInYear)
		{
			year_day -= kDaysInYear;
			++year;
		}
	}
	bool TimeInfo::HasPassed() const
	{
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		bool time_passed =
			(time_info.tm_year >  year) ||
			(time_info.tm_year == year && time_info.tm_yday > year_day) ||
			(time_info.tm_yday == year_day && time_info.tm_hour > hour) ||
			(time_info.tm_hour == hour && time_info.tm_min >= minute);

		return time_passed;
	}

	void TimeInfo::CalculateNumberOfDaysInYear()
	{
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		kDaysInYear = GetNumberOfDaysInYear(time_info.tm_year + 1900);
	}

} // namespace sm