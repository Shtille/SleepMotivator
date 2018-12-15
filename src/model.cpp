#include "model.h"

namespace sm {

	Model::Model(View * view)
	: view_(view)
	, updating_(false)
	, enabled_(true)
	{
	}
	Model::~Model()
	{
	}
	bool Model::Initialize()
	{
		return LoadTriggers();
	}
	void Model::Deinitialize()
	{
	}
	void Model::Update()
	{
		if (updating_)
			return;

		updating_ = true;
		for (auto& trigger : triggers_)
		{
			trigger.Run(view_);
		}
		updating_ = false;
	}
	bool Model::enabled() const
	{
		return enabled_;
	}
	void Model::toggle_enabled()
	{
		enabled_ = !enabled_;
	}
	bool Model::LoadTriggers()
	{
		// Currently manually add triggers

		// Dialog trigger
		{
			// First and only add unique actions
			Action * get_up_action = new MessageBoxAction("Get up", "Are you getting up early tommorow?", MessageBoxKind::kYesNo, MessageBoxIcon::kQuestion);
			Action * get_up_time_action = new MessageBoxAction("Get up time", "Are you getting up at 8 am?", MessageBoxKind::kYesNo, MessageBoxIcon::kQuestion);
			Action * chill_action = new MessageBoxAction("Chill out", "Then I won't bother you...", MessageBoxKind::kOk, MessageBoxIcon::kInformation);
			Action * time_pick_action = new MessageBoxAction("Choose time", "Time picking dialog", MessageBoxKind::kOkCancel, MessageBoxIcon::kWarning);
			Action * time_change_action = new MessageBoxAction("Time change", "<Time change function call>", MessageBoxKind::kOk, MessageBoxIcon::kWarning);
			Action * notification_action = new NotificationAction("Notification", "The program has started its job");
			Action * activate_action = new TriggerEnableAction(triggers_, 1, true);

			// Construct the tree
			ActionTree tree;

			tree.Add(get_up_action);
			tree.Add(get_up_time_action);
			tree.Add(chill_action);
			tree.Add(time_pick_action);
			tree.Add(time_change_action);
			tree.Add(notification_action);
			tree.Add(activate_action);

			tree.SetRoot(get_up_action);
			tree.SetDescendants(get_up_action, get_up_time_action, chill_action);
			tree.SetDescendants(get_up_time_action, notification_action, time_pick_action);
			tree.SetDescendants(time_pick_action, time_change_action, notification_action);
			tree.SetDescendants(time_change_action, notification_action, nullptr);
			tree.SetDescendants(notification_action, activate_action, nullptr);

			// Finally add the only trigger
			triggers_.emplace_back(Trigger(new DummyEvent(), tree));
		}
		// Shutdown notification trigger
		{
			// First and only add unique actions
			Action * shutdown_notification_action = new NotificationAction("Be ready", "The system shutdown is in 10 minutes!");
			Action * shutdown_activation_action = new TriggerEnableAction(triggers_, 2, true);

			// Construct the tree
			ActionTree tree;

			tree.Add(shutdown_notification_action);
			tree.Add(shutdown_activation_action);

			tree.SetRoot(shutdown_notification_action);
			tree.SetDescendants(shutdown_notification_action, shutdown_activation_action, nullptr);

			// Finally add the only trigger
			triggers_.emplace_back(Trigger(new TimePassedEvent(1, 10), tree, false));
		}
		// Shutdown trigger
		{
			// First and only add unique actions
			Action * shutdown_action = new ShutdownAction();

			// Construct the tree
			ActionTree tree;

			tree.Add(shutdown_action);

			tree.SetRoot(shutdown_action);

			// Finally add the only trigger
			triggers_.emplace_back(Trigger(new TimePassedEvent(1, 10), tree, false));
		}
		return true;
	}

} // namespace sm