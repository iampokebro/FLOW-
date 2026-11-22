#pragma once

#define NOMINMAX

#include <iostream>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <fstream>

#include <Windows.h>

#include "include/cef_resource_handler.h"
#include "include/cef_response.h"
#include "include/cef_request.h"
#include "include/cef_callback.h"

#include "include/cef_scheme.h"

#include "include/cef_parser.h"

#include "../../../Backend/DataManager.h"

extern DataManager DM;

class NanoQueryHandler : public CefResourceHandler {
private:
	size_t offset = 0;
	std::string response_data_;
	int64_t offset_ = 0;
	IMPLEMENT_REFCOUNTING(NanoQueryHandler);
public:
	inline bool Open(CefRefPtr<CefRequest> request, bool& handel_request, CefRefPtr<CefCallback> callback) {
		handel_request = true;

		std::string url = request->GetURL();
		CefURLParts parts;
		CefParseURL(url, parts);
		std::string querry = CefString(&parts.query).ToString();

		try {
			DM.Manage(querry, response_data_);
		}
		catch (const std::exception& e) {
			response_data_ = "C++ Error: " + std::string(e.what());
		}

		callback->Continue();
		return true;
	}

	inline void GetResponseHeaders(CefRefPtr<CefResponse> response,
		int64_t& response_length,
		CefString& redirectURL) {
		response->SetMimeType("text/html");
		response->SetStatus(200);
		response->SetHeaderByName("Access-Control-Allow-Origin", "*", true);
		response_length = response_data_.length();
	}

	bool Read(void* data_out,
		int bytesToRead,
		int& byteRead,
		CefRefPtr<CefResourceReadCallback> callback) {
		if (offset >= response_data_.size()) {
			byteRead = 0;
			return false;
		}

		int BTC = std::min(bytesToRead, static_cast<int>(response_data_.length() - static_cast<int>(offset)));
		memcpy(data_out, response_data_.c_str() + offset, BTC);

		offset += BTC;
		byteRead = BTC;
		return true;
	}

	void Cancel() override {}
};

class NanoQuerryHandlerFactory : public CefSchemeHandlerFactory {
public:
	CefRefPtr<CefResourceHandler> Create(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request
	) override {
		return new NanoQueryHandler();
	}
private:
	IMPLEMENT_REFCOUNTING(NanoQuerryHandlerFactory);
};

