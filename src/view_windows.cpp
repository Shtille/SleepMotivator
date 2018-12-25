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

#include "view_windows.h"

#include "model.h"

#include "../res/resource.h"

#include <CommCtrl.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

// Enable visual styles
// See https://docs.microsoft.com/en-us/windows/desktop/controls/cookbook-overview for details
#ifdef _WIN64
#pragma comment(linker,"\"/manifestdependency:type='win64' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define MY_TRAY_ICON_ID							0x1234
#define CONTEXT_MENU_ITEM__ENABLE				0x1235
#define CONTEXT_MENU_ITEM__DISABLE				0x1236
#define CONTEXT_MENU_ITEM__SETTINGS_TIME		0x1237
#define CONTEXT_MENU_ITEM__SETTINGS_DURATION	0x1238
#define CONTEXT_MENU_ITEM__SETTINGS_STURTUP		0x1239
#define CONTEXT_MENU_ITEM__EXIT					0x1240
#define IDT_CHECK_TIMER							0x1

#define NOTIFICATION_TRAY_ICON_MSG (WM_USER + 0x100)

namespace {
	const char * kApplicationClassName = "Sleep Motivator";
	const char * kApplicationTitle = "Sleep motivator";

	sm::WinView g_view;
}

namespace sm {

	WinView::WinView()
	: model_(nullptr)
	, window_(NULL)
	, menu_(NULL)
	, active_icon_(NULL)
	, passive_icon_(NULL)
	{
	}
	WinView::~WinView()
	{
	}
	void WinView::Notification(const std::string & title, const std::string & message)
	{
		ShowTrayBaloon(title, message);
	}
	MessageBoxResult WinView::MessageBox(const std::string & title, const std::string & message, MessageBoxKind kind, MessageBoxIcon icon)
	{
		UINT type = MB_TOPMOST;
		switch (kind)
		{
		case MessageBoxKind::kOk:
			type |= MB_OK;
			break;
		case MessageBoxKind::kOkCancel:
			type |= MB_OKCANCEL;
			break;
		case MessageBoxKind::kYesNo:
			type |= MB_YESNO;
			break;
		default: // just to silence possible warning
			type |= MB_OK;
			break;
		}
		switch (icon)
		{
		case MessageBoxIcon::kNone:
			break;
		case MessageBoxIcon::kInformation:
			type |= MB_ICONINFORMATION;
			break;
		case MessageBoxIcon::kQuestion:
			type |= MB_ICONQUESTION;
			break;
		case MessageBoxIcon::kWarning:
			type |= MB_ICONWARNING;
			break;
		case MessageBoxIcon::kError:
			type |= MB_ICONERROR;
			break;
		}
		int result = MessageBoxA(window_, message.c_str(), title.c_str(), type);
		switch (result)
		{
		case IDOK:
			return MessageBoxResult::kOk;
		case IDYES:
			return MessageBoxResult::kYes;
		case IDNO:
			return MessageBoxResult::kNo;
		case IDCANCEL:
			return MessageBoxResult::kCancel;
		default:
			return MessageBoxResult::kOk;
		}
	}
	bool WinView::TimePickDialog(const std::string& title, int * hours_ptr, int * minutes_ptr)
	{
		dialog_data_.title = title;
		dialog_data_.hours_ptr = hours_ptr;
		dialog_data_.minutes_ptr = minutes_ptr;

		return ShowTimeDialog();
	}
	void WinView::ShutdownTheSystem()
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;

		// Get a token for this process. 
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return;

		// Get the LUID for the shutdown privilege. 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

		tkp.PrivilegeCount = 1;  // one privilege to set
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Get the shutdown privilege for this process. 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
			return;

		// Shut down the system and force all applications to close. 
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_FLAG_PLANNED))
			return;

		// Shutdown was successful
	}
	void WinView::DisableModel()
	{
		if (model_->enabled())
			OnDisableClick();
	}
	ATOM WinView::MyRegisterClass(HINSTANCE hInstance)
	{
		WNDCLASSEXA window_class;
		ZeroMemory(&window_class, sizeof(WNDCLASSEXA));
		window_class.cbSize = sizeof(WNDCLASSEXA);
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		window_class.lpfnWndProc = (WNDPROC)(WindowProc);
		window_class.hInstance = hInstance;
		window_class.hbrBackground = (HBRUSH)NULL;
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		window_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		window_class.lpszClassName = kApplicationClassName;
		return RegisterClassExA(&window_class);
	}
	bool WinView::Initialize(HINSTANCE hInstance)
	{
		// Create window
		window_ = CreateWindowA(kApplicationClassName, kApplicationTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

		if (!window_)
			return false;

		// Create settings popup submenu
		submenu_ = CreatePopupMenu();
		AppendMenu(submenu_, MF_STRING, CONTEXT_MENU_ITEM__SETTINGS_TIME, TEXT("Set get up time"));
		AppendMenu(submenu_, MF_STRING, CONTEXT_MENU_ITEM__SETTINGS_DURATION, TEXT("Set sleep duration"));
		AppendMenu(submenu_, MF_STRING, CONTEXT_MENU_ITEM__SETTINGS_STURTUP, TEXT("Loading on startup"));

		// Create popup menu
		menu_ = CreatePopupMenu();
		AppendMenu(menu_, MF_STRING, CONTEXT_MENU_ITEM__ENABLE, TEXT("Enable"));
		AppendMenu(menu_, MF_STRING, CONTEXT_MENU_ITEM__DISABLE, TEXT("Disable"));
		AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
		AppendMenu(menu_, MF_STRING | MF_POPUP, (UINT_PTR)submenu_, TEXT("Settings"));
		AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
		AppendMenu(menu_, MF_STRING, CONTEXT_MENU_ITEM__EXIT, TEXT("Exit"));

		// Load icons
		active_icon_ = LoadIconA(hInstance, MAKEINTRESOURCE(IDI_SM_ENABLED));
		passive_icon_ = LoadIconA(hInstance, MAKEINTRESOURCE(IDI_SM_DISABLED));

		// The window has been created but not displayed.
		// Now we have a parent window to which a notification tray icon can be associated.
		AddTrayIcon();

		SetTimer(window_, IDT_CHECK_TIMER, 10000, (TIMERPROC)NULL);

		return true;
	}
	void WinView::ShowContextMenu()
	{
		// Get current mouse position.
		POINT cur_point;
		GetCursorPos(&cur_point);

		// should SetForegroundWindow according
		// to original poster so the popup shows on top
		SetForegroundWindow(window_);

		// TrackPopupMenu blocks the app until TrackPopupMenu returns
		UINT clicked = TrackPopupMenu(menu_, 0, cur_point.x, cur_point.y, 0, window_, NULL);
	}
	void WinView::UpdateContextMenu()
	{
		if (model_->enabled())
		{
			EnableMenuItem(menu_, CONTEXT_MENU_ITEM__ENABLE, MF_GRAYED);
			EnableMenuItem(menu_, CONTEXT_MENU_ITEM__DISABLE, MF_ENABLED);
		}
		else // disabled
		{
			EnableMenuItem(menu_, CONTEXT_MENU_ITEM__ENABLE, MF_ENABLED);
			EnableMenuItem(menu_, CONTEXT_MENU_ITEM__DISABLE, MF_GRAYED);
		}
		if (model_->startup_loading())
			CheckMenuItem(submenu_, CONTEXT_MENU_ITEM__SETTINGS_STURTUP, MF_CHECKED);
		else
			CheckMenuItem(submenu_, CONTEXT_MENU_ITEM__SETTINGS_STURTUP, MF_UNCHECKED);
	}
	void WinView::ReadLoadingOnStartupValueFromRegistry()
	{
		HKEY hkey = NULL;
		if (RegOpenKeyA(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &hkey) == ERROR_SUCCESS)
		{
			DWORD dwType;
			if (RegQueryValueExA(hkey, kApplicationClassName, NULL, &dwType, NULL, NULL) == ERROR_SUCCESS)
			{
				// Value exists
				if (!model_->startup_loading())
					model_->toggle_startup_loading();
			}
			RegCloseKey(hkey);
		}
	}
	void WinView::MakeLoadingOnStartup()
	{
		CHAR szFileName[MAX_PATH];
		GetModuleFileNameA(NULL, szFileName, MAX_PATH);

		HKEY hkey = NULL;
		RegCreateKeyA(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &hkey);
		if (model_->startup_loading())
			RegSetValueExA(hkey, kApplicationClassName, 0, REG_SZ, (BYTE*)szFileName, strlen(szFileName) * sizeof(CHAR));
		else
			RegDeleteValueA(hkey, kApplicationClassName);
		RegCloseKey(hkey);
	}
	bool WinView::Load()
	{
		model_ = new Model(this);
		if (!model_->Initialize())
		{
			delete model_;
			return false;
		}

		// Read registry value for startup loading
		ReadLoadingOnStartupValueFromRegistry();

		// Update menu items due to model loading
		UpdateContextMenu();

		return true;
	}
	void WinView::Unload()
	{
		model_->Deinitialize();
		delete model_;
	}
	void WinView::EndWindow()
	{
		KillTimer(window_, IDT_CHECK_TIMER);
		DestroyIcon(passive_icon_);
		DestroyIcon(active_icon_);
		DestroyMenu(menu_);
		DestroyWindow(window_);
	}
	void WinView::AddTrayIcon()
	{
		ZeroMemory(&ni_data_, sizeof(NOTIFYICONDATA));
		ni_data_.cbSize = sizeof(NOTIFYICONDATA);
		ni_data_.uID = MY_TRAY_ICON_ID;
		ni_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		ni_data_.hIcon = active_icon_;
		ni_data_.hWnd = window_;
		ni_data_.dwInfoFlags = NIIF_INFO;
		ni_data_.uTimeout = 10000;
		ni_data_.uVersion = NOTIFYICON_VERSION_4;
		strncpy_s(ni_data_.szTip, kApplicationTitle, ARRAYSIZE(ni_data_.szTip));
		strncpy_s(ni_data_.szInfoTitle, "Title for balloon", ARRAYSIZE(ni_data_.szInfoTitle));
		strncpy_s(ni_data_.szInfo, "Body for balloon", ARRAYSIZE(ni_data_.szInfo));
		ni_data_.uCallbackMessage = NOTIFICATION_TRAY_ICON_MSG;
		Shell_NotifyIcon(NIM_ADD, &ni_data_);
	}
	void WinView::RemoveTrayIcon()
	{
		Shell_NotifyIcon(NIM_DELETE, &ni_data_);
	}
	void WinView::UpdateTrayIcon()
	{
		HICON icon = (model_ && model_->enabled()) ? active_icon_ : passive_icon_;
		ni_data_.hIcon = icon;
		ni_data_.uFlags = NIF_ICON;
		Shell_NotifyIcon(NIM_MODIFY, &ni_data_);
	}
	void WinView::ShowTrayBaloon(const std::string& title, const std::string& message)
	{
		strncpy_s(ni_data_.szInfoTitle, title.c_str(), ARRAYSIZE(ni_data_.szInfoTitle));
		strncpy_s(ni_data_.szInfo, message.c_str(), ARRAYSIZE(ni_data_.szInfo));
		ni_data_.uFlags = NIF_INFO;
		Shell_NotifyIcon(NIM_MODIFY, &ni_data_);
	}
	bool WinView::ShowTimeDialog()
	{
		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(window_, GWL_HINSTANCE);
		return DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_TIME_DIALOG), HWND_DESKTOP, (DLGPROC)TimePickProc, (LPARAM)&dialog_data_) == IDOK;
	}
	void WinView::ShowTimeSettingsDialog()
	{
		if (model_->updating())
			return;

		int hours = model_->get_up_hours();
		int minutes = model_->get_up_minutes();

		dialog_data_.title = TEXT("Change get up time");
		dialog_data_.hours_ptr = &hours;
		dialog_data_.minutes_ptr = &minutes;

		if (ShowTimeDialog())
		{
			model_->set_get_up_hours(hours);
			model_->set_get_up_minutes(minutes);
		}
		// Finally invalidate pointers
		dialog_data_.hours_ptr = nullptr;
		dialog_data_.minutes_ptr = nullptr;
	}
	void WinView::ShowDurationSettingsDialog()
	{
		if (model_->updating())
			return;

		int hours = model_->sleep_duration_hours();
		int minutes = model_->sleep_duration_minutes();

		dialog_data_.title = TEXT("Change sleep duration");
		dialog_data_.hours_ptr = &hours;
		dialog_data_.minutes_ptr = &minutes;

		if (ShowTimeDialog())
		{
			model_->set_sleep_duration_hours(hours);
			model_->set_sleep_duration_minutes(minutes);
		}
		// Finally invalidate pointers
		dialog_data_.hours_ptr = nullptr;
		dialog_data_.minutes_ptr = nullptr;
	}
	void WinView::OnTimer()
	{
		model_->Update();
	}
	void WinView::OnEnableClick()
	{
		model_->toggle_enabled();
		UpdateContextMenu();
		UpdateTrayIcon();
	}
	void WinView::OnDisableClick()
	{
		model_->toggle_enabled();
		UpdateContextMenu();
		UpdateTrayIcon();
	}
	void WinView::OnStartupLoadingClick()
	{
		model_->toggle_startup_loading();
		UpdateContextMenu();
		MakeLoadingOnStartup();
	}
	void WinView::OnExitClick()
	{
		if (model_->enabled() && model_->HasNotificationTimePassed())
		{
			// User has decided to cheat the program logics. What a fool!
			MessageBoxA(HWND_DESKTOP, TEXT("You are the smart one! But I'm smarter!"), TEXT("Fool"), MB_OK | MB_ICONWARNING);
			ShutdownTheSystem();
		}
		else
		{
			// Standard exit routine
			RemoveTrayIcon();
			EndWindow();
		}
	}
	LRESULT WinView::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		int wmId, wmEvent;
		switch (uMsg)
		{
		case WM_CLOSE:
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		case WM_PAINT:
			break;
		case WM_KEYUP:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_TIMER:
			switch (wParam)
			{
			case IDT_CHECK_TIMER:
				g_view.OnTimer();
				return 0;
			}
			break;
		case NOTIFICATION_TRAY_ICON_MSG:
			switch (lParam)
			{
			case WM_LBUTTONDBLCLK:
				break;
			case WM_LBUTTONUP:
				break;
			case WM_RBUTTONDOWN:
			case WM_CONTEXTMENU:
				g_view.ShowContextMenu();
				break;
			case NIN_BALLOONHIDE:
				break;
			case NIN_BALLOONSHOW:
				break;
			case NIN_BALLOONTIMEOUT:
				break;
			case NIN_BALLOONUSERCLICK:
				break;
			case NIN_POPUPCLOSE:
				break;
			case NIN_POPUPOPEN:
				break;
			}
			break;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			// Parse the menu selections:
			switch (wmId)
			{
			case CONTEXT_MENU_ITEM__ENABLE:
				g_view.OnEnableClick();
				break;
			case CONTEXT_MENU_ITEM__DISABLE:
				g_view.OnDisableClick();
				break;
			case CONTEXT_MENU_ITEM__SETTINGS_TIME:
				g_view.ShowTimeSettingsDialog();
				break;
			case CONTEXT_MENU_ITEM__SETTINGS_DURATION:
				g_view.ShowDurationSettingsDialog();
				break;
			case CONTEXT_MENU_ITEM__SETTINGS_STURTUP:
				g_view.OnStartupLoadingClick();
				break;
			case CONTEXT_MENU_ITEM__EXIT:
				g_view.OnExitClick();
				break;
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	BOOL WinView::TimePickProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static WinView::DialogData * data = NULL;
		LONG ex_style;
		HWND control_hwnd;
		TCHAR buffer[8];
		RECT rc;
		int x, y;

		switch (message)
		{
		case WM_INITDIALOG:
			data = (WinView::DialogData *)lParam;
			// Make it top window
			ex_style = GetWindowLongA(hwndDlg, GWL_EXSTYLE);
			ex_style |= WS_EX_TOPMOST;
			SetWindowLongA(hwndDlg, GWL_EXSTYLE, ex_style);
			// Center dialog window
			GetWindowRect(hwndDlg, &rc);
			x = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
			y = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
			SetWindowPos(hwndDlg, HWND_TOP, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			// Dialog caption
			SetWindowText(hwndDlg, data->title.c_str());
			// Hours slider
			control_hwnd = GetDlgItem(hwndDlg, IDC_HOURS_SLIDER);
			SendMessage(control_hwnd, TBM_SETRANGE,
				(WPARAM)TRUE,				// redraw flag 
				(LPARAM)MAKELONG(0, 23));	// min. & max. positions
			SendMessage(control_hwnd, TBM_SETPOS,
				(WPARAM)TRUE,				// redraw flag
				(LPARAM)*data->hours_ptr);	// initial position
			// Minutes slider
			control_hwnd = GetDlgItem(hwndDlg, IDC_MINUTES_SLIDER);
			SendMessage(control_hwnd, TBM_SETRANGE,
				(WPARAM)TRUE,				// redraw flag 
				(LPARAM)MAKELONG(0, 59));	// min. & max. positions
			SendMessage(control_hwnd, TBM_SETPOS,
				(WPARAM)TRUE,				// redraw flag 
				(LPARAM)*data->minutes_ptr);// initial position
			// Set text for edit and static labels
			sprintf_s(buffer, "%i", *data->hours_ptr);
			SetDlgItemText(hwndDlg, IDC_HOURS_STATIC, buffer);
			sprintf_s(buffer, "%i", *data->minutes_ptr);
			SetDlgItemText(hwndDlg, IDC_MINUTES_STATIC, buffer);
			sprintf_s(buffer, "%i:%02i", *data->hours_ptr, *data->minutes_ptr);
			SetDlgItemText(hwndDlg, IDC_TIME_EDIT, buffer);
			break;
		case WM_HSCROLL:
		case WM_VSCROLL:
			switch (LOWORD(wParam))
			{
			case TB_THUMBPOSITION:
			case TB_THUMBTRACK:
				switch (GetDlgCtrlID((HWND)lParam))
				{
				case IDC_HOURS_SLIDER:
					*data->hours_ptr = HIWORD(wParam);
					sprintf_s(buffer, "%i", *data->hours_ptr);
					SetDlgItemText(hwndDlg, IDC_HOURS_STATIC, buffer);
					sprintf_s(buffer, "%i:%02i", *data->hours_ptr, *data->minutes_ptr);
					SetDlgItemText(hwndDlg, IDC_TIME_EDIT, buffer);
					break;
				case IDC_MINUTES_SLIDER:
					*data->minutes_ptr = HIWORD(wParam);
					sprintf_s(buffer, "%i", *data->minutes_ptr);
					SetDlgItemText(hwndDlg, IDC_MINUTES_STATIC, buffer);
					sprintf_s(buffer, "%i:%02i", *data->hours_ptr, *data->minutes_ptr);
					SetDlgItemText(hwndDlg, IDC_TIME_EDIT, buffer);
					break;
				}
				break;
			case TB_ENDTRACK:
				switch (GetDlgCtrlID((HWND)lParam))
				{
				case IDC_HOURS_SLIDER:
					control_hwnd = GetDlgItem(hwndDlg, IDC_HOURS_SLIDER);
					*data->hours_ptr = SendMessage(control_hwnd, TBM_GETPOS, 0, 0);
					sprintf_s(buffer, "%i", *data->hours_ptr);
					SetDlgItemText(hwndDlg, IDC_HOURS_STATIC, buffer);
					sprintf_s(buffer, "%i:%02i", *data->hours_ptr, *data->minutes_ptr);
					SetDlgItemText(hwndDlg, IDC_TIME_EDIT, buffer);
					break;
				case IDC_MINUTES_SLIDER:
					control_hwnd = GetDlgItem(hwndDlg, IDC_MINUTES_SLIDER);
					*data->minutes_ptr = SendMessage(control_hwnd, TBM_GETPOS, 0, 0);
					sprintf_s(buffer, "%i", *data->minutes_ptr);
					SetDlgItemText(hwndDlg, IDC_MINUTES_STATIC, buffer);
					sprintf_s(buffer, "%i:%02i", *data->hours_ptr, *data->minutes_ptr);
					SetDlgItemText(hwndDlg, IDC_TIME_EDIT, buffer);
					break;
				}
				break;
			}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				EndDialog(hwndDlg, wParam);
				return TRUE;
			case IDCANCEL:
				EndDialog(hwndDlg, wParam);
				return TRUE;
			}
		}
		return FALSE;
	}

} // namespace sm

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Prevent from multiply instances
	HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	DWORD dwMutexWaitResult = WaitForSingleObject(hMutex, 0);
	if (dwMutexWaitResult != WAIT_OBJECT_0)
	{
		MessageBoxA(HWND_DESKTOP, TEXT("This application is already running"), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
		CloseHandle(hMutex);
		return 4;
	}

	// Register a window class
	if (g_view.MyRegisterClass(hInstance) == 0)
	{
		MessageBoxA(HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	// Perform application initialization
	if (!g_view.Initialize(hInstance))
	{
		UnregisterClassA(kApplicationClassName, hInstance);
		return 2;
	}

	if (!g_view.Load())
	{
		g_view.EndWindow();
		UnregisterClassA(kApplicationClassName, hInstance);
		return 3;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	g_view.Unload();

	UnregisterClassA(kApplicationClassName, hInstance);

	return (int)msg.wParam;
}