#define NOMINMAX

#include <iostream>
#include <cstring>
#include <filesystem>

#include <Windows.h>

#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>

std::string DllFolder = "Special/";

#include "Background Process Window/CefSystem.hpp"

#include "Backend/DataManager.h"
#include "Backend/Functionlity/FlyCompiler/flyCompiler.hpp"
#include "Backend/Cores/SmartFunction/SmartFunction.h"

using namespace std::literals;

constexpr size_t WND_SIZE_W = 1000;
constexpr size_t WND_SIZE_H = 800;
constexpr bool DEBUG_M = true;

std::string MLF = "appData/Externals/Global Base/";
std::vector<std::string> LoadFiles  = { MLF + "G1.hpp" };
std::string LocalSaveFile("appData/Backend/AST/Base.json");

FlyCompiler FC{};
DataManager DM{};

CefRefPtr<NanoClient> client(new NanoClient);

LRESULT CALLBACK WndProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#include "Background Process Window/WndOthers.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	CefMainArgs mainArgs(hInstance);
	CefRefPtr<NanoApp> pApp(new NanoApp);

	if (const auto code = CefExecuteProcess(mainArgs, pApp.get(), nullptr); code >= 0) {
		exit((int)code);
	}

	{
		CefSettings settings;
		settings.no_sandbox = true;
		settings.multi_threaded_message_loop = true;
		{
			std::string path = (std::filesystem::current_path() / "cef_cache"s).string();
			CefString(&settings.cache_path).FromString(path);
		}

		CefInitialize(mainArgs, settings, pApp.get(), nullptr);
	}

	const char* WndClass = "$CEF_WND$";
	RegWndClass(WndClass, hInstance);

	HWND window = CreateWindowEx(
		0, WndClass,
		"FLOW++",
		WS_OVERLAPPEDWINDOW | WS_BORDER,
		200, 200, WND_SIZE_W, WND_SIZE_H,
		NULL, NULL,
		hInstance,
		NULL
	);

	if (window == NULL) {
		CefShutdown();
		std::cerr << "Failed to create window\n";
		return 1;
	}

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	
	setUpBrowser(window, "app://local/appData/Frontend/index.html");

	//ReSizeWnd(window);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	CefShutdown();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE: {
		if (DEBUG_M) {
			AllocConsole();
			FILE* fOut = nullptr;
			freopen_s(&fOut, "CONOUT$", "w", stdout);
			freopen_s(&fOut, "CONOUT$", "w", stderr);
		}

		DM.LA.parseHeaderFiles(LoadFiles, LocalSaveFile, true);
		DM.init();
		

		break;
	}
	case WM_SIZE: {
		if (client && client->GetBrowser()) {
			ReSizeWnd(hWnd);
		}
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}