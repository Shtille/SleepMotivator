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

#include "event.h"

#include <ctime>

namespace sm {

	DummyEvent::DummyEvent()
	{
	}
	bool DummyEvent::Passed()
	{
		return true;
	}

	TimePassedEvent::TimePassedEvent(const TimeInfo& time_info)
	{
		time_info_ = time_info;
	}
	bool TimePassedEvent::Passed()
	{
		return time_info_.HasPassed();
	}

	ShutdownNotificationTimeEvent::ShutdownNotificationTimeEvent(
		const int * get_up_hour, const int * get_up_minute,
		const int * sleep_duration_hours, const int * sleep_duration_minutes,
		const int * notification_minutes)
	: get_up_hour_(get_up_hour)
	, get_up_minute_(get_up_minute)
	, sleep_duration_hours_(sleep_duration_hours)
	, sleep_duration_minutes_(sleep_duration_minutes)
	, notification_minutes_(notification_minutes)
	{
	}
	bool ShutdownNotificationTimeEvent::Passed()
	{
		// Calculate shutdown notification time
		TimeInfo notification_time;
		notification_time.MakeWakeUp(*get_up_hour_, *get_up_minute_);
		notification_time.MinusHours(*sleep_duration_hours_);
		notification_time.MinusMinutes(*sleep_duration_minutes_);
		notification_time.MinusMinutes(*notification_minutes_);

		return notification_time.HasPassed();
	}

	ShutdownTimeEvent::ShutdownTimeEvent(
		const int * get_up_hour, const int * get_up_minute,
		const int * sleep_duration_hours, const int * sleep_duration_minutes)
	: get_up_hour_(get_up_hour)
	, get_up_minute_(get_up_minute)
	, sleep_duration_hours_(sleep_duration_hours)
	, sleep_duration_minutes_(sleep_duration_minutes)
	{
	}
	bool ShutdownTimeEvent::Passed()
	{
		// Calculate shutdown time
		TimeInfo shutdown_time;
		shutdown_time.MakeWakeUp(*get_up_hour_, *get_up_minute_);
		shutdown_time.MinusHours(*sleep_duration_hours_);
		shutdown_time.MinusMinutes(*sleep_duration_minutes_);

		return shutdown_time.HasPassed();
	}

} // namespace sm