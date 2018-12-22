#ifndef __MODEL_H__
#define __MODEL_H__

#include "trigger.h"

#include <vector>

namespace sm {

	class Model {
	public:
		Model(View * view);
		~Model();

		bool Initialize();
		void Deinitialize();

		void Update();

		bool enabled() const;
		void toggle_enabled();
		void set_get_up_hours(int value);
		void set_get_up_minutes(int value);
		void set_sleep_duration_hours(int value);
		void set_sleep_duration_minutes(int value);

		int get_up_hours() const;
		int get_up_minutes() const;
		int sleep_duration_hours() const;
		int sleep_duration_minutes() const;

	private:

		bool LoadTriggers();

		View * view_;
		std::vector<Trigger> triggers_;
		int get_up_hour_;
		int get_up_minute_;
		int sleep_duration_hours_;
		int sleep_duration_minutes_;
		int chosen_hour_;
		int chosen_minute_;
		int notification_minutes_; // notification time before the shutdown takes place
		bool updating_; // trick to not let spam multiply messages
		bool enabled_;
	};

} // namespace sm

#endif