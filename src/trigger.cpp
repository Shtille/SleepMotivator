#include "trigger.h"

#include <queue>

namespace sm {

	Trigger::Trigger()
	: event_(nullptr)
	, action_(nullptr)
	, enabled_(false)
	, repeatable_(false)
	{
	}
	Trigger::Trigger(Event * event, Action * action)
	: event_(event)
	, action_(action)
	, enabled_(true)
	, repeatable_(false)
	{
	}
	Trigger::Trigger(const Trigger & other)
	: event_(other.event_)
	, action_(other.action_)
	, enabled_(other.enabled_)
	, repeatable_(other.repeatable_)
	{
	}
	Trigger::Trigger(Trigger && other)
	: event_(other.event_)
	, action_(other.action_)
	, enabled_(other.enabled_)
	, repeatable_(other.repeatable_)
	{
		other.event_ = nullptr;
		other.action_ = nullptr;
	}
	Trigger::~Trigger()
	{
		if (event_)
			delete event_;

		// Action is represented by entire tree
		std::queue<Action*> action_queue; // queue is the best container to process tree
		if (action_)
			action_queue.push(action_); // initial value
		while (!action_queue.empty())
		{
			// Pop the first element
			Action * action = action_queue.front();
			action_queue.pop();

			// Push only valid values
			if (action->next_first_)
				action_queue.push(action->next_first_);
			if (action->next_second_)
				action_queue.push(action->next_second_);

			// Finally release it
			delete action;
		}
	}
	void Trigger::Run(View * view)
	{
		if (!enabled_)
			return;

		if (event_ && event_->Passed())
		{
			// Traverse action tree
			Action * action = action_;
			while (action)
				action = action->Execute(view);

			// Disable the trigger
			if (!repeatable_)
				enabled_ = false;
		}
	}
	void Trigger::set_enabled(bool enabled)
	{
		enabled_ = enabled;
	}
	void Trigger::set_repeatable(bool repeatable)
	{
		repeatable_ = repeatable;
	}

} // namespace sm