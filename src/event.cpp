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

	TimePassedEvent::TimePassedEvent(int hour, int minute)
	{
		time_info_.hour = hour;
		time_info_.minute = minute;
	}
	bool TimePassedEvent::Passed()
	{
		time_t raw_time;
		tm time_info;

		time(&raw_time);
		localtime_s(&time_info, &raw_time);

		bool time_passed = (time_info.tm_hour > time_info_.hour) ||
			(time_info.tm_hour == time_info_.hour && time_info.tm_min >= time_info_.minute);

		return time_passed;
	}

} // namespace sm