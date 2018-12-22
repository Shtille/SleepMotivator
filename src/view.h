/*************************************************************************
* Sleep Motivator
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

#ifndef __VIEW_H__
#define __VIEW_H__

#include "message_box_kind.h"
#include "message_box_icon.h"
#include "message_box_result.h"
#include "time_info.h"

#include <string>

namespace sm {

	class View {
	public:
		virtual ~View() = default;

		virtual void Notification(const std::string& title, const std::string& message) = 0;
		virtual MessageBoxResult MessageBox(const std::string& title, const std::string& message, MessageBoxKind kind, MessageBoxIcon icon) = 0;
		virtual bool TimePickDialog(const std::string& title, int * hours_ptr, int * minutes_ptr) = 0; // returns true on <OK> and false on <Cancel>
		virtual void ShutdownTheSystem() = 0;
	};

} // namespace sm

#endif