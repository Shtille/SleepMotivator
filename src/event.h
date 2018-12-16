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