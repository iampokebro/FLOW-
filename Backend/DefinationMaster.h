#pragma once

#include <cstring>
#include <string>

#include <vector>


/// querry = "Call=Func&para={1, 2, 3}&type={int, float, int}&FName=Addition"
inline void defFunc(std::string& querry, std::string& respone, HandlerMasterFunction& HMF) {

	querry = CefURIDecode(querry, true, static_cast<cef_uri_unescape_rule_t>(
		UU_NORMAL | UU_SPACES |
		UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS |
		UU_PATH_SEPARATORS
		)).ToString();

	std::vector<std::any> parameters = ExtractPara(querry);
	std::string FName = ExtractPhrase(querry, "FName=");
	
	auto it = HMF.Handler.find("Global");
	if (it == HMF.Handler.end()) return;

	respone = anyToString(it->second.Execute(FName, parameters));
	return;
}
///"Call=iFunc&FName=Name&FGroup=NULL&RType=int&Import=a long string&Code=A Long String"
inline void initSFunc(std::string& querry, std::string& respone, HandlerSmartFunction& HSF, HandlerMasterFunction& HMF) {
	std::string FuncName = ExtractPhrase(querry, "FName=");
	std::string FuncGroup = ExtractPhrase(querry, "FGroup=");
	std::string retType = ExtractPhrase(querry, "RType=");
	std::string Imports = ExtractPhrase(querry, "Import=");
	std::string Code = ExtractPhrase(querry, "Code=");

	Replace(Imports, "%0A", "\n");
	Replace(Code, "%0A", "\n");

	if (!Imports.empty())
	Imports = CefURIDecode(Imports, true, static_cast<cef_uri_unescape_rule_t>(
		UU_NORMAL | UU_SPACES |
		UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS |
		UU_PATH_SEPARATORS
		)).ToString();
	
	if (!Code.empty());
	Code = CefURIDecode(Code, true, static_cast<cef_uri_unescape_rule_t>(
		UU_NORMAL | UU_SPACES |
		UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS |
		UU_PATH_SEPARATORS
		)).ToString();

	if (HSF.addFunction(FuncName, FuncGroup, retType, Imports, Code, HMF.getHandlerRef("FUNCTION"))) {
		respone = "Successful";
	}
	else respone = "Unable To Register Function";
}

///"Call=sFunc&FName=Name&LAdress=uintptr_t"
inline void exeSFunc(std::string& querry, std::string& respone, HandlerSmartFunction& HSF, HandlerMasterFunction& HMF) {
	std::string FuncName = ExtractPhrase(querry, "FName=");
	int LAdress = std::stoi(ExtractPhrase(querry, "LAdress="));

	HSF.exeFunction(FuncName, static_cast<uintptr_t>(LAdress), HMF.getHandlerRef("FUNCTION"), respone);
	return;
}

///Call=lSpace&SName=Local1&CMD=C;
//CMD = C create
//CMD = D delte
//CMD = R reset
//CMD = P pointer in uint_t
//CMD = G get
//CMD = S set
inline void LocalSpace(std::string& querry, std::string& response, HandlerMasterVar& HMV) {
	std::string cmd = ExtractPhrase(querry, "CMD=");
	std::string sname = ExtractPhrase(querry, "SName=");

	if (cmd == "C") {
		auto it = HMV.Handler.find(sname);
		if (it != HMV.Handler.end()) {
			response = "Local Space Already Exisit : " + sname;
			return;
		}
		HMV.addHandler(sname);
	}
	else if (cmd == "D") {
		auto it = HMV.Handler.find(sname);
		if (it == HMV.Handler.end()) return;
		HMV.Handler.erase(sname);
	}
	else if (cmd == "R") {
		auto it = HMV.Handler.find(sname);
		if (it == HMV.Handler.end()) {
			return;
		}
		HMV.Handler[sname].Handler.clear();
	}
	else if (cmd == "P") {
		auto it = HMV.Handler.find(sname);
		if (it == HMV.Handler.end()) {
			return;
		}

		response = std::to_string( reinterpret_cast<uintptr_t>( &it->second ) );
	}

	response = "Successful";
}

///Call=lvSpace&SName=Local1&VName=Var1&CMD=C&Type=int&Val=4
inline void LVarSpace(std::string& querry, std::string& response, HandlerMasterVar& HMV) {
	std::string sname = ExtractPhrase(querry, "SName=");
	std::string vname = ExtractPhrase(querry, "VName=");
	std::string cmd = ExtractPhrase(querry, "CMD=");


	auto it = HMV.Handler.find(sname);
	if (it == HMV.Handler.end()) {
		response = "No Space Exisit " + sname;
		return;
	}
	auto &space = HMV.Handler[sname];

	if (cmd == "C") {
		auto ct = space.Handler.find(vname);
		if (ct != space.Handler.end()) {
			return;
		}

		std::string type = ExtractPhrase(querry, "Type=");
		std::string val = ExtractPhrase(querry, "Val=");

		space.createVar(vname, type, false);
	}
	else if (cmd == "D") {
		if (!space.isVarExisits(vname)) return;
		
		space.Handler.erase(vname);
	}
	else if (cmd == "G") {
		auto ct = space.Handler.find(vname);
		if (ct == space.Handler.end()) {
			return;
		}

		if (ct->second.type == static_cast<uint8_t>(VarIds::Int)) response = ct->second.Int;
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Float)) response = ct->second.Float;
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Double)) response = ct->second.Double;
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Char)) response = ct->second.Char;
		else if (ct->second.type == static_cast<uint8_t>(VarIds::String)) response = ct->second.String;
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Bool)) response = ct->second.Bool;
		return;
	}
	else if (cmd == "S") {
		auto ct = space.Handler.find(vname);
		if (ct == space.Handler.end()) {
			return;
		}

		std::string val = ExtractPhrase(querry, "Val=");

		if (ct->second.type == static_cast<uint8_t>(VarIds::Int)) ct->second.setVar(std::stoi(val));
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Float)) ct->second.setVar(std::stof(val));
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Double)) ct->second.setVar(std::stod(val));
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Char)) ct->second.setVar(val[0]);
		else if (ct->second.type == static_cast<uint8_t>(VarIds::String)) ct->second.setVar(val.c_str());
		else if (ct->second.type == static_cast<uint8_t>(VarIds::Bool)) ct->second.setVar(parseBool(val));
	}

	response = "Successful";
}


