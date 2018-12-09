#ifndef __ACTION_H__
#define __ACTION_H__

#include "message_box_kind.h"
#include "message_box_icon.h"

#include <string>

namespace sm {

	// Forward declarations
	class View;

	//! Base action class
	class Action {
		friend class Trigger;
	public:
		virtual ~Action() = default;

		virtual Action * Execute(View * view) = 0;

		void set_next_first(Action * next);
		void set_next_second(Action * next);

	protected:
		enum Type {
			kNotification,	// baloon notification
			kMessageBox		// standard message box
		};

		Action(Type type);
		Action(const Action& other) = delete;
		Action(const Action&& other) = delete;

		Action * next_first_;
		Action * next_second_;

	private:
		Type type_;
	};

	//! Notification action class
	class NotificationAction : public Action {
	public:
		NotificationAction(const std::string& title, const std::string& message);

		virtual Action * Execute(View * view) final;

	private:
		std::string title_;
		std::string message_;
	};

	//! Message box action class
	class MessageBoxAction : public Action {
	public:
		MessageBoxAction(const std::string& title, const std::string& message, MessageBoxKind answer_kind, MessageBoxIcon icon);

		virtual Action * Execute(View * view) final;

	private:
		std::string title_;
		std::string message_;
		MessageBoxKind answer_kind_;
		MessageBoxIcon icon_;
	};

} // namespace sm

#endif