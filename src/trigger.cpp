#include "trigger.h"

namespace sm {

	Trigger::Trigger()
	: event_(nullptr)
	, action_tree_()
	, enabled_(false)
	, repeatable_(false)
	{
	}
	Trigger::Trigger(Event * event, ActionTree & action_tree, bool enabled)
	: event_(event)
	, action_tree_(std::move(action_tree))
	, enabled_(enabled)
	, repeatable_(false)
	{
	}
	Trigger::Trigger(Trigger && other)
	: event_(std::move(other.event_))
	, action_tree_(std::move(other.action_tree_))
	, enabled_(other.enabled_)
	, repeatable_(other.repeatable_)
	{
	}
	Trigger::~Trigger()
	{
	}
	void Trigger::Run(View * view)
	{
		if (!enabled_)
			return;

		if (event_ && event_->Passed())
		{
			// Traverse action tree
			Action * action = action_tree_.root();
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