#pragma once

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

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static BOOL CALLBACK TimePickProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		Model * model_;

		HWND window_;
		HMENU menu_;
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