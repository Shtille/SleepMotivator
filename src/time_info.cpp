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