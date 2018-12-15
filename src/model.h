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
		bool updating_; // trick to not let spam multiply messages
		bool enabled_;
	};

} // namespace sm

#endif