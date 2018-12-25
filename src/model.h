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

#ifndef __MODEL_H__
#define __MODEL_H__

#include "trigger.h"
#include "ini_file.h"

#include <vector>

namespace sm {

	class Model {
		friend class IniFileReader;
	public:
		Model(View * view);
		~Model();

		bool Initialize();
		void Deinitialize();

		void Update();

		bool HasNotificationTimePassed() const;

		bool updating() const;
		bool enabled() const;
		bool startup_loading() const;
		void toggle_startup_loading();
		void toggle_enabled();
		void set_get_up_hours(int value);
		void set_get_up_minutes(int value);
		void set_sleep_duration_hours(int value);
		void set_sleep_duration_minutes(int value);

		int get_up_hours() const;
		int get_up_minutes() const;
		int sleep_duration_hours() const;
		int sleep_duration_minutes() const;

	private:

		bool LoadParameters();
		void SaveParameters();
		bool LoadTriggers();

		class IniFileReader : public IniFileReaderInterface {
		public:
			IniFileReader(Model * model);

			virtual void OnSection(const char* section_name) final;
			virtual void OnPair(const char* key, const char* value) final;

		private:
			Model * model_;
		};

		View * view_;
		IniFile ini_file_;
		std::vector<Trigger> triggers_;
		int get_up_hour_;
		int get_up_minute_;
		int sleep_duration_hours_;
		int sleep_duration_minutes_;
		int chosen_hour_;
		int chosen_minute_;
		int notification_minutes_; // notification time before the shutdown takes place
		bool updating_; // trick to not let spam multiply messages
		bool enabled_;
		bool startup_loading_; // whether laoding on system startup
	};

} // namespace sm

#endif