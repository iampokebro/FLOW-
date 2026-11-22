#pragma once
#include <cstdint>
#include <vector>
#include <string>

enum class VarIds : uint8_t {
    Char = 0,
    Int = 1,
    Float = 2,
    Double = 3,
    String = 4,
    Bool = 5
};

inline std::vector<std::string> VarTypes = {
    "char", "int", "float", "double", "string", "bool"
};
