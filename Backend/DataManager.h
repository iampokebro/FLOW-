#pragma once

#include <iostream>
#include <string>

#include "Global Handlers/HandlerMasterFunction.h";
#include "Global Handlers/MasterHandlerVar.h"
#include "Global Handlers/HandlerSmartFunction.h"

#include "Cores/SmartFunction/StringParser.h"
#include "DefinationMaster.h"

#include "Functionlity/LoaderAbstract/LoaderAbstract.h";

/// Calls
/// Func = Normal Function
/// sFunc = Smart Function
/// Var = Variables
/// Calls

struct DataManager {
public:

	HandlerMasterFunction HMF{}; ///Grand Father Of Function Handler
	HandlerMasterVar HMV{}; /// Grand Father Of Varaiable Handler
	HandlerSmartFunction HSF{}; /// Father Of Smart Function
	LoaderAbstract LA{}; // Global Abstract Unit

	void init() {
		HMF.defineGlobal();
		HMV.defineGlobal();
	}

	void Manage(std::string& querry, std::string& respone) {
		std::string Call = ExtractPhrase(querry, "Call=");
		if (Call == "Func") defFunc(querry, respone, HMF); // normal function note
		else if (Call == "iFunc") initSFunc(querry, respone, HSF, HMF);
		else if (Call == "sFunc") exeSFunc(querry, respone, HSF, HMF);
		else if (Call == "lSpace") LocalSpace(querry, respone, HMV);
		else if (Call == "lvSpace") LVarSpace(querry, respone, HMV);
		else {
			//std::cout << "No Call Defination! " << querry << "\n";
			return;
		}
	}

};