#pragma once

#define NOMINMAX

#include <iostream>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <fstream>

#include "include/cef_resource_handler.h"
#include "include/cef_callback.h"

#include "include/cef_scheme.h"

#include "include/cef_parser.h"

using namespace std::literals;

class NanoResoruceHandler : public CefResourceHandler {
public:
	bool Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback) override {
		handle_request = true;

		std::string url = request->GetURL();
		CefURLParts parts;
		if (!CefParseURL(url, parts)) return false;

		std::string rpath = CefString(&parts.path);
		if (rpath[0] == '/') rpath = rpath.substr(1);

		if (rpath.empty() || rpath == "/") rpath = "index.html";;

		filepath = (std::filesystem::current_path() / rpath).string();
		std::cout << "Attempting to load: " << filepath << " | Exists: " << std::filesystem::exists(filepath) << "\n";

		if (std::filesystem::exists(filepath)) {
			_fileStream.open(filepath, std::ios::binary | std::ios::ate);
			if (_fileStream.is_open()) {
				fileSize = _fileStream.tellg();
				_fileStream.seekg(0, std::ios::beg);
				Opened = true;
			}
		}

		callback->Continue();

		return Opened;
	}

	void GetResponseHeaders(CefRefPtr<CefResponse> response, int64_t& response_length, CefString& redirectURL) override {

		std::string mime = "text/html";
		if (filepath.extension() == ".css") {
			mime = "text/css";
		}
		else if (filepath.extension() == ".js") {
			mime = "application/javascript";
		}
		else if (filepath.extension() == ".json") {
			mime = "application/json";
		}
		else if (filepath.extension() == ".png") {
			mime = "image/png";
		}
		else if (filepath.extension() == ".jpg" || filepath.extension() == ".jpeg") {
			mime = "image/jpeg";
		}
		response->SetMimeType(mime);

		response->SetStatus(200);
		response_length = fileSize;


		CefResponse::HeaderMap headerMap;
		response->GetHeaderMap(headerMap);

		headerMap.insert(std::make_pair("Access-Control-Allow-Origin", "*"));
		headerMap.insert(std::make_pair("Access-Control-Allow-Methods", "GET, POST, OPTIONS"));
		headerMap.insert(std::make_pair("Access-Control-Allow-Headers", "*"));

		response->SetHeaderMap(headerMap);
	}

	bool Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback) override {
		if (!Opened || !_fileStream.is_open() || _fileStream.eof()) {
			bytes_read = 0;
			return false;
		}

		_fileStream.read(static_cast<char*>(data_out), bytes_to_read);
		bytes_read = static_cast<int>(_fileStream.gcount());
		return bytes_read > 0;
	}

	void Cancel() override {
		if (_fileStream.is_open()) {
			_fileStream.close();
		}
	}


private:
	std::ifstream _fileStream;
	int64_t fileSize = 0;
	bool Opened = false;
	std::filesystem::path filepath;

	IMPLEMENT_REFCOUNTING(NanoResoruceHandler);
};

class NanoSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
	CefRefPtr<CefResourceHandler> Create(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request
	) override {
		return new NanoResoruceHandler();
	}
private:
	IMPLEMENT_REFCOUNTING(NanoSchemeHandlerFactory);
};
