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

#ifndef __VIEW_WINDOWS_H__
#define __VIEW_WINDOWS_H__

#include "view.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <shellapi.h>

#ifdef MessageBox
#undef MessageBox
#endif

namespace sm {

	// Forward decalrations
	class Model;

	//! The main point of impl class is not to abuse windows headers
	class WinView final : public View {
	public:
		WinView();
		~WinView();

		// Derived from View class
		virtual void Notification(const std::string& title, const std::string& message) final;
		virtual MessageBoxResult MessageBox(const std::string& title, const std::string& message, MessageBoxKind kind, MessageBoxIcon icon) final;
		virtual bool TimePickDialog(const std::string& title, int * hours_ptr, int * minutes_ptr) final;
		virtual void ShutdownTheSystem() final;

		ATOM MyRegisterClass(HINSTANCE hInstance);
		bool Initialize(HINSTANCE hInstance);

		bool Load();
		void Unload();

		void EndWindow();

	protected:
		void ShowContextMenu();
		void UpdateContextMenu();

		void MakeLoadingOnStartup();

		void AddTrayIcon();
		void RemoveTrayIcon();
		void UpdateTrayIcon();
		void ShowTrayBaloon(const std::string& title, const std::string& message);

		bool ShowTimeDialog();
		void ShowTimeSettingsDialog();
		void ShowDurationSettingsDialog();

		void OnTimer();
		void OnEnableClick();
		void OnDisableClick();
		void OnStartupLoadingClick();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static BOOL CALLBACK TimePickProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		Model * model_;

		HWND window_;
		HMENU menu_;
		HMENU submenu_;
		HICON active_icon_;
		HICON passive_icon_;
		NOTIFYICONDATA ni_data_;

		struct DialogData {
			std::string title;
			int * hours_ptr;
			int * minutes_ptr;
		};
		DialogData dialog_data_;
	};

} // namespace sm

#endif