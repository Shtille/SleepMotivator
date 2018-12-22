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

#ifndef __EVENT_H__
#define __EVENT_H__

#include "time_info.h"

namespace sm {

	//! Base event class
	class Event {
	public:
		Event() = default;
		virtual ~Event() = default;

		virtual bool Passed() = 0;

	private:
		Event(const Event& other) = delete;
		Event(const Event&& other) = delete;
	};

	//! Dummy event class
	class DummyEvent : public Event {
	public:
		DummyEvent();

		virtual bool Passed() final;
	};

	//! Time based event class
	// Passes when time elapses stored value
	class TimePassedEvent : public Event {
	public:
		TimePassedEvent(const TimeInfo& time_info);

		virtual bool Passed() final;

	private:
		TimeInfo time_info_;
	};

	//! Shutdown notification time event class
	class ShutdownNotificationTimeEvent : public Event {
	public:
		ShutdownNotificationTimeEvent(const int * get_up_hour, const int * get_up_minute,
			const int * sleep_duration_hours, const int * sleep_duration_minutes,
			const int * notification_minutes);

		virtual bool Passed() final;

	private:
		const int * get_up_hour_;
		const int * get_up_minute_;
		const int * sleep_duration_hours_;
		const int * sleep_duration_minutes_;
		const int * notification_minutes_;
	};

	//! Shutdown time event class
	class ShutdownTimeEvent : public Event {
	public:
		ShutdownTimeEvent(const int * get_up_hour, const int * get_up_minute,
			const int * sleep_duration_hours, const int * sleep_duration_minutes);

		virtual bool Passed() final;

	private:
		const int * get_up_hour_;
		const int * get_up_minute_;
		const int * sleep_duration_hours_;
		const int * sleep_duration_minutes_;
	};

} // namespace sm

#endif