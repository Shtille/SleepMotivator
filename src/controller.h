#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

namespace sm {

	// Forward declarations
	class Model;

	class Controller {
	public:
		Controller(Model * model);
		~Controller();

	protected:

	private:
		Model * model_; // pointer to model object
	};

} // namespace sm

#endif