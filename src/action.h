#ifndef __ACTION_H__
#define __ACTION_H__

#include "message_box_kind.h"
#include "message_box_icon.h"
#include "time_info.h"

#include <memory>
#include <string>
#include <vector>

namespace sm {

	// Forward declarations
	class View;
	class Trigger;

	//! Base action class
	class Action {
		friend class ActionTree;
	public:
		virtual ~Action() = default;

		virtual Action * Execute(View * view);

	protected:

		Action() = default;
		Action(const Action& other) = delete;
		Action(Action&& other) = delete;

		Action * next_first_;
		Action * next_second_;
	};

	//! Class made for trigger directly to store entire action tree
	class ActionTree {
	public:
		ActionTree();
		ActionTree(Action * root);

		void Add(Action * action);
		void SetRoot(Action * root);
		void SetDescendants(Action * root, Action * left, Action * right);

		Action * root();

	private:
		std::vector< std::unique_ptr<Action> > actions_list_; //! list to store unique actions
		Action * action_root_; //! raw pointer that points the root of the tree, nodes may have many links
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

	//! Enabling trigger action class
	class TriggerEnableAction : public Action {
	public:
		TriggerEnableAction(std::vector<Trigger>& triggers, size_t id, bool enable);

		virtual Action * Execute(View * view) final;

	private:
		std::vector<Trigger>& triggers_;
		size_t id_;
		bool enable_;
	};

	//! Time change action class
	class TimeChangeAction : public Action {
	public:
		TimeChangeAction(int * target_hour, int * target_minute, const int * chosen_hour, const int * chosen_minute);

		virtual Action * Execute(View * view) final;

	private:
		int * target_hour_;
		int * target_minute_;
		const int * chosen_hour_;
		const int * chosen_minute_;
	};

	//! System shutdown action class
	class ShutdownAction : public Action {
	public:
		ShutdownAction();

		virtual Action * Execute(View * view) final;
	};

} // namespace sm

#endif