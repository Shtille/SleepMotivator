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
		TimePassedEvent(int hour, int minute);

		virtual bool Passed() final;

	private:
		TimeInfo time_info_;
	};

} // namespace sm

#endif