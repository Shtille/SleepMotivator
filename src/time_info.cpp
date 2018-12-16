#include "time_info.h"

#include <ctime>

namespace sm {

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
					// TODO: Add check 365 or 366 days per year
					year_day += 366;
					--year;
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
				// TODO: Add check 365 or 366 days per year
				year_day += 366;
				--year;
			}
		}
	}
	void TimeInfo::MinusDays(int days)
	{
		year_day -= days;
		if (year_day < 0)
		{
			// TODO: Add check 365 or 366 days per year
			year_day += 366;
			--year;
		}
	}
	void TimeInfo::PlusDays(int days)
	{
		year_day += days;
		if (year_day > 365) // or 364?
		{
			year_day -= 366;
			++year;
		}
	}

} // namespace sm