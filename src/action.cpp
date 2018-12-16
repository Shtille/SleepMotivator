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
		return nullptr;
	}

	ShutdownAction::ShutdownAction()
	{
	}
	Action * ShutdownAction::Execute(View * view)
	{
		// Temporary disable it :)
		//view->ShutdownTheSystem();
		return nullptr;
	}

} // namespace sm