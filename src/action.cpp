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

#include "action.h"

#include "trigger.h"
#include "view.h"

namespace sm {

	Action * Action::Execute(View * view)
	{
		return nullptr;
	}

	ActionTree::ActionTree()
	: action_root_(nullptr)
	{
	}
	ActionTree::ActionTree(Action * root)
	: action_root_(root)
	{
	}
	void ActionTree::Add(Action * action)
	{
		actions_list_.push_back(std::unique_ptr<Action>(action));
	}
	void ActionTree::SetRoot(Action * root)
	{
		action_root_ = root;
	}
	void ActionTree::SetDescendants(Action * root, Action * left, Action * right)
	{
		root->next_first_ = left;
		root->next_second_ = right;
	}
	Action * ActionTree::root()
	{
		return action_root_;
	}

	NotificationAction::NotificationAction(const std::string& title, const std::string& message)
	: title_(title)
	, message_(message)
	{
	}
	Action * NotificationAction::Execute(View * view)
	{
		view->Notification(title_, message_);
		return next_first_;
	}

	MessageBoxAction::MessageBoxAction(const std::string& title, const std::string& message, MessageBoxKind answer_kind, MessageBoxIcon icon)
	: title_(title)
	, message_(message)
	, answer_kind_(answer_kind)
	, icon_(icon)
	{
	}
	Action * MessageBoxAction::Execute(View * view)
	{
		MessageBoxResult result = view->MessageBox(title_, message_, answer_kind_, icon_);
		if (result == MessageBoxResult::kNo || result == MessageBoxResult::kCancel)
			return next_second_;
		else // kYes or kOk
			return next_first_;
	}

	TimePickDialogAction::TimePickDialogAction(const std::string & title, int * hours_ptr, int * minutes_ptr)
	: title_(title)
	, hours_ptr_(hours_ptr)
	, minutes_ptr_(minutes_ptr)
	{
	}
	Action * TimePickDialogAction::Execute(View * view)
	{
		if (view->TimePickDialog(title_, hours_ptr_, minutes_ptr_)) // OK
			return next_first_;
		else // Cancel
			return next_second_;
	}

	TriggerEnableAction::TriggerEnableAction(std::vector<Trigger>& triggers, size_t id, bool enable)
	: triggers_(triggers)
	, id_(id)
	, enable_(enable)
	{
	}
	Action * TriggerEnableAction::Execute(View * view)
	{
		triggers_[id_].set_enabled(enable_);
		return nullptr;
	}

	TimeChangeAction::TimeChangeAction(int * target_hour, int * target_minute, const int * chosen_hour, const int * chosen_minute)
	: target_hour_(target_hour)
	, target_minute_(target_minute)
	, chosen_hour_(chosen_hour)
	, chosen_minute_(chosen_minute)
	{
	}
	Action * TimeChangeAction::Execute(View * view)
	{
		*target_hour_ = *chosen_hour_;
		*target_minute_ = *chosen_minute_;
		return next_first_;
	}

	ShutdownAction::ShutdownAction()
	{
	}
	Action * ShutdownAction::Execute(View * view)
	{
		// Temporary disable it :)
		//view->ShutdownTheSystem();
		return next_first_;
	}

} // namespace sm