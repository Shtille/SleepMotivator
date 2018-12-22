/*************************************************************************
*  Sleep Motivator
* ------------------------------------------------------------------------
*  Copyright (c) 2018 Vladimir Sviridov <v.shtille@gmail.com>
*
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
*
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
*
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
*
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
*
*  3. This notice may not be removed or altered from any source
*     distribution.
*
**************************************************************************/

#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "event.h"
#include "action.h"

#include <memory>
#include <vector>

namespace sm {

	class Trigger {
	public:
		Trigger();
		Trigger(Event * event, ActionTree & action_tree, bool enabled = true);
		Trigger(Trigger&& other);
		~Trigger();

		void Run(View * view);

		void set_enabled(bool enabled);
		void set_repeatable(bool repeatable);

	private:
		Trigger(const Trigger& other) = delete;

		std::unique_ptr<Event> event_; //!< event is unique
		ActionTree action_tree_; //!< but the action can be a tree actually
		bool enabled_;		// default is true
		bool repeatable_;	// default is false
	};

} // namespace sm

#endif