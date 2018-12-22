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

#ifndef __TIME_INFO_H__
#define __TIME_INFO_H__

namespace sm {

	struct TimeInfo {
		int minute;		// minutes after the hour - [0, 59]
		int hour;		// hours since midnight - [0, 23]
		int year_day;	// days since January 1 - [0, 365]
		int year;		// years since 1900

		void Make(int hours, int minutes);
		void MakeWakeUp(int hours, int minutes);

		void MinusMinutes(int minutes);
		void MinusHours(int hours);
		void MinusDays(int days);
		void PlusDays(int days);
	};

} // namespace sm

#endif