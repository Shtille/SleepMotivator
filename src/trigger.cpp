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