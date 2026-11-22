#pragma once

#include <cstring>
#include <iostream>
#include <unordered_map>

#include "../Functionlity/ManagerVariable/ManagerVariable.h"

struct HandlerMasterVar {
public:
	std::unordered_map<std::string, ManagerVariable> Handler;

	inline void addHandler(std::string name) {
		Handler[name] = ManagerVariable{};
	}

	inline ManagerVariable getHandler(std::string name) {
		return Handler[name];
	}

	inline void defineGlobal() {
		addHandler("Global");
	}
};