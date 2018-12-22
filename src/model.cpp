#include "model.h"

namespace {
	const char * kConfigFile = "config.ini";
}

namespace sm {

	Model::Model(View * view)
	: view_(view)
	, get_up_hour_(8)
	, get_up_minute_(0)
	, sleep_duration_hours_(8)
	, sleep_duration_minutes_(0)
	, chosen_hour_(8)
	, chosen_minute_(0)
	, notification_minutes_(10)
	, updating_(false)
	, enabled_(true)
	, startup_loading_(false)
	{
	}
	Model::~Model()
	{
	}
	bool Model::Initialize()
	{
		if (!LoadParameters())
			return false;
		if (!LoadTriggers())
			return false;
		return true;
	}
	void Model::Deinitialize()
	{
		SaveParameters();
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
	bool Model::updating() const
	{
		return updating_;
	}
	bool Model::enabled() const
	{
		return enabled_;
	}
	bool Model::startup_loading() const
	{
		return startup_loading_;
	}
	void Model::toggle_startup_loading()
	{
		startup_loading_ = !startup_loading_;
	}
	void Model::toggle_enabled()
	{
		enabled_ = !enabled_;
	}
	void Model::set_get_up_hours(int value)
	{
		get_up_hour_ = value;
	}
	void Model::set_get_up_minutes(int value)
	{
		get_up_minute_ = value;
	}
	void Model::set_sleep_duration_hours(int value)
	{
		sleep_duration_hours_ = value;
	}
	void Model::set_sleep_duration_minutes(int value)
	{
		sleep_duration_minutes_ = value;
	}
	int Model::get_up_hours() const
	{
		return get_up_hour_;
	}
	int Model::get_up_minutes() const
	{
		return get_up_minute_;
	}
	int Model::sleep_duration_hours() const
	{
		return sleep_duration_hours_;
	}
	int Model::sleep_duration_minutes() const
	{
		return sleep_duration_minutes_;
	}
	bool Model::LoadParameters()
	{
		if (!ini_file_.OpenForRead(kConfigFile))
		{
			// We gonna use default parameters then
			return true;
		}
		IniFileReader reader(this);
		ini_file_.Read(&reader);
		ini_file_.Close();
		// Finally update dialog values
		chosen_hour_ = get_up_hour_;
		chosen_minute_ = get_up_minute_;
		return true;
	}
	void Model::SaveParameters()
	{
		if (!ini_file_.OpenForWrite(kConfigFile))
			return;
		ini_file_.WritePair("version", "2");
		ini_file_.WritePair("get_up_hours", std::to_string(get_up_hour_).c_str());
		ini_file_.WritePair("get_up_minutes", std::to_string(get_up_minute_).c_str());
		ini_file_.WritePair("sleep_duration_hours", std::to_string(sleep_duration_hours_).c_str());
		ini_file_.WritePair("sleep_duration_minutes", std::to_string(sleep_duration_minutes_).c_str());
		ini_file_.WritePair("startup_loading", (startup_loading_) ? "true" : "false");
		ini_file_.Close();
	}
	bool Model::LoadTriggers()
	{
		// Currently manually add triggers

		// Dialog trigger
		{
			char buffer[64];
			snprintf(buffer, 64, "Are you getting up at %i:%02i ?", get_up_hour_, get_up_minute_);
			std::string get_up_string(buffer);

			// First and only add unique actions
			Action * get_up_action = new MessageBoxAction("Get up", "Are you getting up early tommorow?", MessageBoxKind::kYesNo, MessageBoxIcon::kQuestion);
			Action * get_up_time_action = new MessageBoxAction("Get up time", get_up_string, MessageBoxKind::kYesNo, MessageBoxIcon::kQuestion);
			Action * chill_action = new MessageBoxAction("Chill out", "Then I won't bother you...", MessageBoxKind::kOk, MessageBoxIcon::kInformation);
			Action * time_pick_action = new TimePickDialogAction("Choose get up time", &chosen_hour_, &chosen_minute_);
			Action * time_change_action = new TimeChangeAction(&get_up_hour_, &get_up_minute_, &chosen_hour_, &chosen_minute_);
			Action * notification_action = new NotificationAction("The motivation has begun", "The program has started its job for motivating you!");
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
			std::string notification_string = std::string("The system shutdown is in ")
				+ std::to_string(notification_minutes_) + std::string(" minutes!");

			// First and only add unique actions
			Action * shutdown_notification_action = new NotificationAction("Be ready", notification_string);
			Action * shutdown_activation_action = new TriggerEnableAction(triggers_, 2, true);

			// Construct the tree
			ActionTree tree;

			tree.Add(shutdown_notification_action);
			tree.Add(shutdown_activation_action);

			tree.SetRoot(shutdown_notification_action);
			tree.SetDescendants(shutdown_notification_action, shutdown_activation_action, nullptr);

			Event * event = new ShutdownNotificationTimeEvent(&get_up_hour_, &get_up_minute_,
				&sleep_duration_hours_, &sleep_duration_minutes_, &notification_minutes_);

			// Finally add the only trigger
			triggers_.emplace_back(Trigger(event, tree, false));
		}
		// Shutdown trigger
		{
			// First and only add unique actions
			Action * shutdown_action = new ShutdownAction();

			// Construct the tree
			ActionTree tree;

			tree.Add(shutdown_action);

			tree.SetRoot(shutdown_action);

			Event * event = new ShutdownTimeEvent(&get_up_hour_, &get_up_minute_,
				&sleep_duration_hours_, &sleep_duration_minutes_);

			// Finally add the only trigger
			triggers_.emplace_back(Trigger(event, tree, false));
		}
		return true;
	}

	Model::IniFileReader::IniFileReader(Model * model)
	: model_(model)
	{
	}
	void Model::IniFileReader::OnSection(const char * section_name)
	{
		// Skip section
	}
	void Model::IniFileReader::OnPair(const char * key, const char * value)
	{
		if (strcmp(key, "version") == 0)
		{
			// Skip version
		}
		else if (strcmp(key, "get_up_hours") == 0)
			model_->get_up_hour_ = ::atoi(value);
		else if (strcmp(key, "get_up_minutes") == 0)
			model_->get_up_minute_ = ::atoi(value);
		else if (strcmp(key, "sleep_duration_hours") == 0)
			model_->sleep_duration_hours_ = ::atoi(value);
		else if (strcmp(key, "sleep_duration_minutes") == 0)
			model_->sleep_duration_minutes_ = ::atoi(value);
		else if (strcmp(key, "startup_loading") == 0)
			model_->startup_loading_ = (strcmp(value, "true") == 0);
	}

} // namespace sm