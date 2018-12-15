#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

namespace sm {

	// Forward declarations
	class Model;
	class View;

	class Controller {
	public:
		Controller(Model * model, View * view);
		~Controller();

	protected:

	private:
		Model * model_; // pointer to model object
		View * view_; // pointer to view object
	};

} // namespace sm

#endif