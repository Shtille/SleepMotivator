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

	//! Time pick dialog class
	class TimePickDialogAction : public Action {
	public:
		TimePickDialogAction(const std::string& title, int * hours_ptr, int * minutes_ptr);

		virtual Action * Execute(View * view) final;

	private:
		std::string title_;
		int * hours_ptr_;
		int * minutes_ptr_;
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