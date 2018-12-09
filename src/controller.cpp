#include "controller.h"

#include "model.h"

#include <cmath>

namespace sm {

	Controller::Controller(Model * model)
		: model_(model)
	{
	}
	Controller::~Controller()
	{
	}

} // namespace sm