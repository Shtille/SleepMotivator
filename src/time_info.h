#ifndef __TIME_INFO_H__
#define __TIME_INFO_H__

namespace sm {

	struct TimeInfo {
		int minute;		// minutes after the hour - [0, 59]
		int hour;		// hours since midnight - [0, 23]
		int year_day;	// days since January 1 - [0, 365]
		int year;		// years since 1900

		void Make(int hours, int minutes);
		void MakeWakeUp(int hours, int minutes);

		void MinusMinutes(int minutes);
		void MinusHours(int hours);
		void MinusDays(int days);
		void PlusDays(int days);
	};

} // namespace sm

#endif