#pragma once
#include <Windows.h>

#include <cstring>
extern CefRefPtr<NanoClient> client;

inline void RegWndClass(const char* WndClass, HINSTANCE hInstance) {
	{
		WNDCLASS wc = {};
		wc.lpfnWndProc = WndProcess;
		wc.hInstance = hInstance;
		wc.lpszClassName = WndClass;
		wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

		RegisterClass(&wc);
	}
}

inline void setUpBrowser(HWND window, std::string link) {
	RECT rect;
	GetClientRect(window, &rect);

	CefWindowInfo wndInfo;
	CefBrowserSettings bSetting;
	CefRect cRect;
	cRect.width = rect.right - rect.left;
	cRect.height = rect.bottom - rect.top;
	cRect.x = rect.left;
	cRect.y = rect.top;

	bSetting.javascript = STATE_ENABLED;
	bSetting.javascript_access_clipboard = STATE_ENABLED;

	wndInfo.SetAsChild(window, cRect);
	CefBrowserHost::CreateBrowser(wndInfo, client, link, bSetting, nullptr, nullptr);
}


inline void ReSizeWnd(HWND hWnd) {
	CefWindowHandle hwndBrowser = client->GetBrowser()->GetHost()->GetWindowHandle();

	RECT rect;
	GetClientRect(hWnd, &rect);
	SetWindowPos(hwndBrowser, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
}