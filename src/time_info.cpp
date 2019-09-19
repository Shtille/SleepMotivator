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

	TimeInfo::TimeInfo()
	{

	}
	TimeInfo::TimeInfo(const TimeInfo& other)
		: time_point_(other.time_point_)
	{

	}
	void TimeInfo::MakeCurrent()
	{
		time_point_ = Clock::now();
	}
	void TimeInfo::Make(int hours, int minutes)
	{
		std::time_t raw_time;
		std::tm time_info;

		time_point_ = Clock::now();
		raw_time = Clock::to_time_t(time_point_);
		localtime_s(&time_info, &raw_time);
		time_info.tm_hour = hours;
		time_info.tm_min = minutes;
		raw_time = std::mktime(&time_info);
		time_point_ = Clock::from_time_t(raw_time);
	}
	void TimeInfo::MakeWakeUp(int hours, int minutes)
	{
		std::time_t raw_time;
		std::tm time_info;

		time_point_ = Clock::now();
		raw_time = Clock::to_time_t(time_point_);
		localtime_s(&time_info, &raw_time);
		bool next_day = time_info.tm_hour > hours;
		time_info.tm_hour = hours;
		time_info.tm_min = minutes;
		raw_time = std::mktime(&time_info);
		time_point_ = Clock::from_time_t(raw_time);
		if (next_day)
			time_point_ += std::chrono::hours(24); // +1 day
	}
	void TimeInfo::MinusMinutes(int minutes)
	{
		time_point_ -= std::chrono::minutes(minutes);
	}
	void TimeInfo::PlusMinutes(int minutes)
	{
		time_point_ += std::chrono::minutes(minutes);
	}
	void TimeInfo::MinusHours(int hours)
	{
		time_point_ -= std::chrono::hours(hours);
	}
	bool TimeInfo::HasPassed() const
	{
		Clock::time_point current_time_point = Clock::now();
		return time_point_ <= current_time_point;
	}

} // namespace sm