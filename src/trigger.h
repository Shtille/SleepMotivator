#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "event.h"
#include "action.h"

#include <memory>
#include <vector>

namespace sm {

	class Trigger {
	public:
		Trigger();
		Trigger(Event * event, ActionTree & action_tree, bool enabled = true);
		Trigger(Trigger&& other);
		~Trigger();

		void Run(View * view);

		void set_enabled(bool enabled);
		void set_repeatable(bool repeatable);

	private:
		Trigger(const Trigger& other) = delete;

		std::unique_ptr<Event> event_; //!< event is unique
		ActionTree action_tree_; //!< but the action can be a tree actually
		bool enabled_;		// default is true
		bool repeatable_;	// default is false
	};

} // namespace sm

#endif