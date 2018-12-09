#ifndef __TIME_INFO_H__
#define __TIME_INFO_H__

namespace sm {

	struct TimeInfo {
		int hour;	// hours since midnight - [0, 23]
		int minute;	// minutes after the hour - [0, 59]
	};

} // namespace sm

#endif