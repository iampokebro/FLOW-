#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>

#include "../Cores/SmartFunction/SmartFunction.h";
#include "../Functionlity/FlyCompiler/flyCompiler.hpp"

extern FlyCompiler FC;

struct HandlerSmartFunction {
	std::unordered_map<std::string, Function> Handler;

public:
	bool addFunction(std::string& FuncName, std::string& FuncGroup, std::string& retType, std::string& Imports, std::string& Code, ManagerBase& MB) {
		
		///Maybe Three Conditions Like Not Found, Found and Ret Type and GType Updated
		///i replace the found with if update, and it works the same, maybe i assume.
		auto it = Handler.find(FuncName);
		if (it != Handler.end()) { ///Its a Problem Cause is DLL be Update or What
			auto Fn = it->second;
			Fn.FuncGroup = FuncGroup;
			Fn.retType = retType;
			Fn.Imports = Imports;
			Fn.Code = Code;
			Fn.Compile(1, 1);
			Fn.parseImport();
			return FC.compileFunction(MB, FuncName, FuncGroup, Fn.Code);
		}

		Function Fn{};
		Fn.FuncName = FuncName;
		Fn.FuncGroup = FuncGroup;
		Fn.retType = retType;
		Fn.Imports = Imports;
		Fn.Code = Code;
		Fn.parseImport();
		Fn.Compile(1, 1);

		//std::cout << Fn.Code << "\n";
		
		if (!FC.compileFunction(MB, FuncName, FuncGroup, Fn.Code)) return false;
		else Handler[FuncName] = Fn;
		return true;
	}

	void exeFunction(std::string& FuncName, uintptr_t LocalAddress, ManagerBase& MB, std::string& response) {

		auto it = Handler.find(FuncName);
		if (it == Handler.end()) return;

		it->second.Local = reinterpret_cast<ManagerVariable*>(LocalAddress);
		it->second.initManagerVar();

		response = anyToString(MB.Execute(FuncName, {}));
		return;
	}
};