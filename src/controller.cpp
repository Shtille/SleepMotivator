#include "controller.h"

#include "model.h"

#include <cmath>

namespace sm {

	Controller::Controller(Model * model, View * view)
	: model_(model)
	, view_(view)
	{
	}
	Controller::~Controller()
	{
	}

} // namespace sm