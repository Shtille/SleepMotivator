#pragma once

#include "view.h"

#include <Windows.h>
#include <shellapi.h>

#ifdef MessageBox
#undef MessageBox
#endif

namespace sm {

	// Forward decalrations
	class Model;
	class Controller;

	//! The main point of impl class is not to abuse windows headers
	class WinView final : public View {
	public:
		WinView();
		~WinView();

		// Derived from View class
		virtual void Notification(const std::string& title, const std::string& message) final;
		virtual MessageBoxResult MessageBox(const std::string& title, const std::string& message, MessageBoxKind kind, MessageBoxIcon icon) final;
		virtual void TimeSelectionBox(const TimeInfo * initial_time, TimeInfo * result_time) final;

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

		void OnTimer();
		void OnEnableClick();
		void OnDisableClick();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		Model * model_;
		Controller * controller_;

		HWND window_;
		HMENU menu_;
		HICON active_icon_;
		HICON passive_icon_;
		NOTIFYICONDATA ni_data_;
	};

} // namespace sm