#define NOMINMAX
#pragma once

#include <Windows.h>

#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>

#include "CefHandlers/ResoureFileHandler/FileResoureHandler.hpp"
#include "CefHandlers/QuerryHandler/QuerryHandler.h"

class NanoClient : public CefClient, public CefLifeSpanHandler {
private:
	CefRefPtr<CefBrowser> m_browser;
public:
	NanoClient() {};

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
		m_browser = browser;
	}

	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
		m_browser = nullptr;
	}

	CefRefPtr<CefBrowser> GetBrowser() { return m_browser; }

	IMPLEMENT_REFCOUNTING(NanoClient);
};



class NanoApp : public CefApp, public CefBrowserProcessHandler {
public:

	CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	void OnContextInitialized() override {
		CefRegisterSchemeHandlerFactory("app", "local", new NanoSchemeHandlerFactory{});
		CefRegisterSchemeHandlerFactory("app", "func", new NanoQuerryHandlerFactory{});
	}

	void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
		registrar->AddCustomScheme(
			"app",
			CEF_SCHEME_OPTION_STANDARD |
			CEF_SCHEME_OPTION_SECURE |
			CEF_SCHEME_OPTION_CSP_BYPASSING |
			CEF_SCHEME_OPTION_CORS_ENABLED |
			CEF_SCHEME_OPTION_FETCH_ENABLED
		);

	}
public:
	IMPLEMENT_REFCOUNTING(NanoApp);
};