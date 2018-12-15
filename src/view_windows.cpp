#include "view_windows.h"

#include "model.h"
#include "controller.h"

#include "../res/resource.h"

#define MY_TRAY_ICON_ID                     0x1234
#define CONTEXT_MENU_ITEM__ENABLE			0x1235
#define CONTEXT_MENU_ITEM__DISABLE			0x1236
#define CONTEXT_MENU_ITEM__SETTINGS			0x1237
#define CONTEXT_MENU_ITEM__EXIT				0x1238
#define IDT_CHECK_TIMER						0x1

#define NOTIFICATION_TRAY_ICON_MSG (WM_USER + 0x100)

namespace {
	const char * kApplicationClassName = "Sleep Motivator";
	const char * kApplicationTitle = "Sleep motivator";

	sm::WinView g_view;
}

namespace sm {

	WinView::WinView()
	: model_(nullptr)
	, controller_(nullptr)
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
		UINT type;
		switch (kind)
		{
		case MessageBoxKind::kOk:
			type = MB_OK;
			break;
		case MessageBoxKind::kOkCancel:
			type = MB_OKCANCEL;
			break;
		case MessageBoxKind::kYesNo:
			type = MB_YESNO;
			break;
		default: // just to silence possible warning
			type = MB_OK;
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
	void WinView::TimeSelectionBox(const TimeInfo * initial_time, TimeInfo * result_time)
	{
	}
	void WinView::ShutdownTheSystem()
	{
		if (0 == ::InitiateSystemShutdownExA(NULL, "The sleep time has come!", 10, FALSE, FALSE, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_FLAG_PLANNED))
		{
			MessageBoxA(window_, "We've failed to start the shutdown", "So sad", MB_OK | MB_ICONINFORMATION);
		}
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
		window_ = CreateWindowA(kApplicationClassName, kApplicationTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

		if (!window_)
			return false;

		menu_ = CreatePopupMenu();
		AppendMenu(menu_, MF_STRING, CONTEXT_MENU_ITEM__ENABLE, TEXT("Enable"));
		AppendMenu(menu_, MF_STRING, CONTEXT_MENU_ITEM__DISABLE, TEXT("Disable"));
		AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
		AppendMenu(menu_, MF_STRING, CONTEXT_MENU_ITEM__SETTINGS, TEXT("Settings"));
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
	}
	bool WinView::Load()
	{
		model_ = new Model(this);
		if (!model_->Initialize())
		{
			delete model_;
			return false;
		}

		controller_ = new Controller(model_, this);

		// Update menu items due to model loading
		UpdateContextMenu();

		return true;
	}
	void WinView::Unload()
	{
		model_->Deinitialize();
		delete model_;
		delete controller_;
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
			case CONTEXT_MENU_ITEM__SETTINGS:
				break;
			case CONTEXT_MENU_ITEM__EXIT:
				g_view.RemoveTrayIcon();
				g_view.EndWindow();
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

} // namespace sm

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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