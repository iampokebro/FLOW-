#pragma once

#include <cstring>
#include <iostream>
#include <unordered_map>

#include "../Functionlity/ManagerBase/ManagerBase.h"

struct HandlerMasterFunction {
public:
	std::unordered_map<std::string, ManagerBase> Handler;

	inline void addHandler(std::string name) {
		auto it = Handler.find(name);
		if (it != Handler.end()) {
			std::cerr << "Handler Already Exsits " << name << "\n";
			return;
		}
		Handler[name] = ManagerBase{};
	}

	inline ManagerBase getHandler(std::string name) {
		auto it = Handler.find(name);
		if (it == Handler.end()) {
			std::cerr << "Fail to Get Handler " << name << "\n";
			return ManagerBase{};
		}
		return Handler[name];
	}

	inline ManagerBase& getHandlerRef(std::string name) {
		auto it = Handler.find(name);
		if (it == Handler.end()) {
			std::cerr << "Fail to Get Handler " << name << "\n";
			return Handler["Global"];
		}
		return Handler[name];
	}

	void defineGlobal();
};