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
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		bool time_passed =
			(time_info.tm_year > time_info_.year) ||
			(time_info.tm_year == time_info_.year && time_info.tm_yday > time_info_.year_day) ||
			(time_info.tm_yday == time_info_.year_day && time_info.tm_hour > time_info_.hour) ||
			(time_info.tm_hour == time_info_.hour && time_info.tm_min >= time_info_.minute);

		return time_passed;
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
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		// Calculate shutdown notification time
		TimeInfo notification_time;
		notification_time.MakeWakeUp(*get_up_hour_, *get_up_minute_);
		notification_time.MinusHours(*sleep_duration_hours_);
		notification_time.MinusMinutes(*sleep_duration_minutes_);
		notification_time.MinusMinutes(*notification_minutes_);

		bool time_passed =
			(time_info.tm_year > notification_time.year) ||
			(time_info.tm_year == notification_time.year && time_info.tm_yday > notification_time.year_day) ||
			(time_info.tm_yday == notification_time.year_day && time_info.tm_hour > notification_time.hour) ||
			(time_info.tm_hour == notification_time.hour && time_info.tm_min >= notification_time.minute);

		return time_passed;
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
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		// Calculate shutdown time
		TimeInfo shutdown_time;
		shutdown_time.MakeWakeUp(*get_up_hour_, *get_up_minute_);
		shutdown_time.MinusHours(*sleep_duration_hours_);
		shutdown_time.MinusMinutes(*sleep_duration_minutes_);

		bool time_passed =
			(time_info.tm_year > shutdown_time.year) ||
			(time_info.tm_year == shutdown_time.year && time_info.tm_yday > shutdown_time.year_day) ||
			(time_info.tm_yday == shutdown_time.year_day && time_info.tm_hour > shutdown_time.hour) ||
			(time_info.tm_hour == shutdown_time.hour && time_info.tm_min >= shutdown_time.minute);

		return time_passed;
	}

} // namespace sm