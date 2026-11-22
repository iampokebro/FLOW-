#pragma once
#include <iostream>
#include <unordered_map>
#include <any>
#include <utility>
#include <cstring>
#include <cstdio>
#include <type_traits>

#include "FunctionProviderVariable.h"

struct ManagerVariable {
public:

    std::unordered_map<std::string, Var> Handler;
    bool global = false;
    uint16_t scopeId = 0;

    std::string Root = "Return";

    ManagerVariable(bool isGlobal = false, uint16_t ScopeId = 0)
     : global(isGlobal), scopeId(ScopeId) {}

public:
    void setC(std::string Name) {
        Root = Name;
    }
    template<typename T>
    void operator=(T val){
        Var &V = Handler[Root];
        uint8_t t = V.type;

        if constexpr (std::is_arithmetic_v<T>) {
            if (t == static_cast<uint8_t>(VarIds::Int))         V.Int = static_cast<decltype(V.Int)>(val);
            else if (t == static_cast<uint8_t>(VarIds::Float))  V.Float = static_cast<decltype(V.Float)>(val);
            else if (t == static_cast<uint8_t>(VarIds::Double)) V.Double = static_cast<decltype(V.Double)>(val);
            else if (t == static_cast<uint8_t>(VarIds::Bool))   V.Bool = static_cast<bool>(val);
            else if (t == static_cast<uint8_t>(VarIds::Char))   V.Char = static_cast<char>(val);
        }
        else if constexpr (std::is_convertible_v<T, const char*>) {
            if (t == static_cast<uint8_t>(VarIds::String)) V.String = static_cast<const char*>(val);
        }
    }

public:
    template<typename Type>
    void createVar(std::string Name = "", Type&& Value = 0, bool constant = false) {
        if (Name.empty()) {
            std::cout << "Name Empty, InitVar, Normal\n";
            return;
        }
        
        if (isVarExisits(Name)) {
            std::cout << "Variable Already Created " << Name << "\n";
            return;
        }

        Var v = Value;
        v.constant = constant;
        Handler[Name] = v;

        std::cout << isVarExisits(Name) << "\n";
    }
    void createVar(std::string Name = "", std::string Type = "", bool constant = false) {
        uint8_t t = getTypeCode(Type);
        if (t == static_cast<uint8_t>(VarIds::Int)) createVar(Name, 0, constant);
        else if (t == static_cast<uint8_t>(VarIds::String)) createVar(Name, "null", constant);
        else if (t == static_cast<uint8_t>(VarIds::Char)) createVar(Name, 'n', constant);
        else if (t == static_cast<uint8_t>(VarIds::Bool)) createVar(Name, true, constant);
        else if (t == static_cast<uint8_t>(VarIds::Float)) createVar(Name, 0.0f, constant);
        else if (t == static_cast<uint8_t>(VarIds::Double)) createVar(Name, static_cast<double>(0.0f), constant);
    }

    bool isVarExisits(std::string Name) {
        if (Name.empty()) {
            std::cout << "Name Empty, setVar, Normal\n";
            return false;
        }
        auto it = Handler.find(Name);
        if (it == Handler.end()) {
            return false;
        }

        return true;
    }

    template<typename Type>
    void setVar(std::string Name = "", Type Value = 0) {
        if (!isVarExisits(Name)) return;
        if (Handler[Name].constant) {
            std::cout << "Constant Variable " << Name << "\n";
            return;
        }

        Handler[Name].setVar(Value);
    }

    Var getVar(std::string Name) {
        if (!isVarExisits(Name)) return {};
        return Handler[Name];
    }

    void delVar(std::string Name) {
        if (!isVarExisits(Name)) return;

        Handler.erase(Name);
        return;
    }
};
