#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "event.h"
#include "action.h"

namespace sm {

	class Trigger {
	public:
		Trigger();
		Trigger(Event * event, Action * action);
		Trigger(const Trigger& other);
		Trigger(Trigger&& other);
		~Trigger();

		void Run(View * view);

		void set_enabled(bool enabled);
		void set_repeatable(bool repeatable);

	private:
		Event * event_;
		Action * action_;
		bool enabled_;		// default is true
		bool repeatable_;	// default is false
	};

} // namespace sm

#endif