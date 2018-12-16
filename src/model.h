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