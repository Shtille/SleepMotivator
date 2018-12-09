#include "action.h"

#include "view.h"

namespace sm {

	void Action::set_next_first(Action * next)
	{
		next_first_ = next;
	}
	void Action::set_next_second(Action * next)
	{
		next_second_ = next;
	}
	Action::Action(Type type)
	: next_first_(nullptr)
	, next_second_(nullptr)
	, type_(type)
	{
	}

	NotificationAction::NotificationAction(const std::string& title, const std::string& message)
	: Action(Action::kNotification)
	, title_(title)
	, message_(message)
	{
	}
	Action * NotificationAction::Execute(View * view)
	{
		view->Notification(title_, message_);
		return next_first_;
	}

	MessageBoxAction::MessageBoxAction(const std::string& title, const std::string& message, MessageBoxKind answer_kind, MessageBoxIcon icon)
	: Action(Action::kMessageBox)
	, title_(title)
	, message_(message)
	, answer_kind_(answer_kind)
	, icon_(icon)
	{
	}
	Action * MessageBoxAction::Execute(View * view)
	{
		MessageBoxResult result = view->MessageBox(title_, message_, answer_kind_, icon_);
		if (result == MessageBoxResult::kNo)
			return next_second_;
		else // kYes or kOk
			return next_first_;
	}

} // namespace sm