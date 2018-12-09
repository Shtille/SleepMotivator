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

	private:
		bool LoadTriggers();

		View * view_;
		std::vector<Trigger> triggers_;
	};

} // namespace sm

#endif