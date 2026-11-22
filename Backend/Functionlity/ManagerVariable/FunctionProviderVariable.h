#pragma once
#include <iostream>
#include <cstdint>

#include "FunctionProviderVariableList.h"

/*
Whoever reading this, i wanted to create this more like function call and free like the current version
but i was too afraid of typing the lot of overloading function so i decided to use this way
union and handler should not be editable(public) in my dream version,
Header["num1"].Int = 5,
this above i dont want, i want a thing more like godot or python
Manager.get("num"); this would return int autometically,
lets see how far this will go, and would this be a great discussion
                                            ----first one to code....11/10/2026(DD/MM/YYYY)
*/

struct Var {
    uint8_t type = 0;
    bool constant = false;

public:
    union {
        char Char;//0
        int Int;//1
        float Float;//2
        double Double;//3
        const char* String;//4
        bool Bool;
    };

public://initlization
    Var() = default;
    Var (int Val) :  type(static_cast<uint8_t>(VarIds::Int)), Int(Val) {}
    Var (float Val) : type(static_cast<uint8_t>(VarIds::Float)), Float(Val) {}
    Var (double Val) : type(static_cast<uint8_t>(VarIds::Double)), Double(Val) {}
    Var (char Val) : type(static_cast<uint8_t>(VarIds::Char)), Char(Val) {}
    Var (const char* Val) : type(static_cast<uint8_t>(VarIds::String)), String(Val) {}
    Var (bool Val) : type(static_cast<uint8_t>(VarIds::Bool)), Bool(Val) {}

public://seters
    void setVar(int Value = 0) {Int = Value;}
    void setVar(float Value = 0) {Float = Value;}
    void setVar(double Value = 0) {Double = Value;}
    void setVar(char Value = 0) {Char = Value;}
    void setVar(const char* Value = 0) {String = Value;}
    void setVar(bool Value = 0) {Bool = Value;}
};



inline uint8_t getTypeCode(std::string& Name) {
    if (Name == "int") return static_cast<uint8_t>(VarIds::Int);
    else if (Name == "float") return static_cast<uint8_t>(VarIds::Float);
    else if (Name == "double") return static_cast<uint8_t>(VarIds::Double);
    else if (Name == "char") return static_cast<uint8_t>(VarIds::Char);
    else if (Name == "string") return static_cast<uint8_t>(VarIds::String);
    else if (Name == "bool") return static_cast<uint8_t>(VarIds::Bool);

    return static_cast<uint8_t>(VarIds::Int);
}

///Weired the return type are capitalized, its for our SmartFunction.h,
///you future dev, will find that its helps to parse the string easily
inline std::string getTypeName(uint8_t& code) {
    switch (code) {
    case static_cast<uint8_t>(VarIds::Int):
        return "Int";
    case static_cast<uint8_t>(VarIds::Float):
        return "Float";
    case static_cast<uint8_t>(VarIds::Double):
        return "Double";
    case static_cast<uint8_t>(VarIds::Char):
        return "Char";
    case static_cast<uint8_t>(VarIds::String):
        return "String";
    case static_cast<uint8_t>(VarIds::Bool):
        return "Bool";
    default:
        return "Int";
    }
}
