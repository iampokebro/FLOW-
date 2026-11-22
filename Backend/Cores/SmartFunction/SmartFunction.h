#pragma once
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdint>
#include <algorithm>

#include "../../Functionlity/ManagerVariable/ManagerVariable.h"

#include "StringParser.h"

using namespace std::literals;

std::string APath = "/../../Flow++/Backend/Functionlity/ManagerVariable/ManagerVariable.h";

///started at 11/11/2026 DD/MM/YYYY
///ended at 13/11/2026 DD/MM/YY

///user manual,
///create a function area display button and code area
///one input, talking address of local
///one output giving output return, you can use local itself

///i thinks its overall complete

struct ImportData {
    uint8_t type = 0;
    std::string name = "";
};

class Function {
public:
    uint8_t id = 0;
    std::string FuncName = "";
    std::string FuncGroup = "";
    std::string retname = "";
    std::string retType = "int";
    std::string Imports = "";
    std::string Code = "";
    ManagerVariable *Local = nullptr;
    ManagerVariable *Global = nullptr;

    //int, double, float, char, const char*, bool
    std::vector<ImportData> ImportDataS; // name : type according to ManagerVaraiable use VarIds

    ///Remember that i have to move all this to the getWordArray so can give precise location
    void parseImport() {
        //take first word ignore spaces always, and find until the next space,  to get the word its just a state machine
        std::vector<std::string> Parsed = getWordArray(Imports);
        std::vector<ImportData> ImportDataTem;
        for (unsigned int i = 0; i < Parsed.size(); i += 3) {
            ImportData ID{};
            //check for type
            if (std::ranges::find(VarTypes, Parsed[i]) == VarTypes.end()) {
                std::cout << "No Such Varaible " << Parsed[i] << " " << Parsed[i+1] << "\n";
                return;
            }
            ID.type = getTypeCode(Parsed[i]);
            //set the name
            ID.name = Parsed[i+1];
            //check if comma seprated
            if (Parsed[i+1] == ",") {
                std::cout << "No Seprate With Comma " << Parsed[i+1] << "\n";
                return;
            }
            ImportDataTem.push_back(ID);
        }

        ImportDataS = ImportDataTem;
    }

    //Now Parameter are known so storing it in Local ManagerVariable
    void initManagerVar() {
        Local->Handler.clear();
        for (ImportData ID : ImportDataS) {
            auto it = Local->Handler.find(ID.name);
            if (it != Local->Handler.end()) continue;

            Var V{};
            V.type = ID.type;
            Local->Handler[ID.name] = V;
        }
    }

    //dont use before initManagerVar
    void setReturn(std::string Type) {
        retType = Type;
        Var V{};
        V.type = getTypeCode(Type);

        retname = "Return" + std::to_string(static_cast<int>(id));

        if (Local)  Local->Handler[retname] = V;
    }



private://today i have to do something about fly compilation
    //take output and try finding $Var or %Var
    //done this can convert our special c++ to normal c++(working now only for vars)
    void parseFn() {
        size_t start = 0;
        size_t last = 0;
        Code = "ManagerVariable* Global = reinterpret_cast<ManagerVariable*>(" + std::to_string(reinterpret_cast<uintptr_t>(&Global)) + ");\n" +
            "ManagerVariable** LocalSlot = reinterpret_cast<ManagerVariable**>(" + std::to_string(reinterpret_cast<uintptr_t>(&Local)) + ");\n" +
            "ManagerVariable* Local = *LocalSlot;\n" +
            Code;

        while (true) {
            last = findVar(Code, start);
            if (last == std::string::npos || start == std::string::npos || start >= Code.size()) break;

            std::string var = Code.substr(start+1, last - start - 1);
            std::string type = Code.substr(start, 1);

            std::string rept = "";

            if (type == "%") rept = "Local->setC(\"" + var + "\");\n" + "*Local = ";
            else if(type == "$") rept = "Global->setC(\"" + var + "\");\n" + "*Global = ";
            else if (type == "@") rept = "Local->setC(\"Return\");\n" + "*Local = "s;
            

            if (!rept.empty()) {
                Code.replace(start, last - start, rept);
                start += rept.size();
            } else start = last;

        }
    }

    void convertFn() {
        //std::cout << std::filesystem::current_path().string() + APath << "\n";
        Code = "#include \"" + std::filesystem::current_path().string() + APath + 
            "\"\n" + 
            " extern \"C\" __declspec(dllexport) void " + FuncName + "(){ \n " + Code + "  \n}";
    }

public:
    ///Future Developer,
    ///mode = 0 fast mode, where function is skipped completely
    ///mode = 1 memory mode, Normal Mode
    ///mode = 2 skip ParseImport, cause no need of that
    void ParaCompile(uint8_t mode = 1) {
        if (mode == 0) return;
        if (mode != 2) parseImport();//pure compilation
        initManagerVar();//while running
        ImportDataS.clear();
    }

    void FnCompile(uint8_t mode = 1) {
        ///above Local And Global Varaible needs to be declaread to curretly execute the function soo i need to make some changes in function rather then doing it with problem
        setReturn(retType);
        parseFn();
        convertFn();
    }

    void Compile(uint8_t Pmode = 1, uint8_t FNmode = 0) {
        //ParaCompile(Pmode); never use this in code, make it smart
        FnCompile(FNmode);
    }

private:

};
