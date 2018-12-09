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
		Action * first_action = new MessageBoxAction("first", "some text", MessageBoxKind::kYesNo, MessageBoxIcon::kQuestion);
		Action * second_action = new MessageBoxAction("second", "some text", MessageBoxKind::kOk, MessageBoxIcon::kInformation);
		Action * third_action = new MessageBoxAction("third", "some text", MessageBoxKind::kOk, MessageBoxIcon::kWarning);
		first_action->set_next_first(second_action);
		first_action->set_next_second(third_action);
		triggers_.push_back(Trigger(new TimePassedEvent(7, 30), first_action));
		return true;
	}

} // namespace sm