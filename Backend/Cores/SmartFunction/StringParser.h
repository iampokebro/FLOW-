#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <climits>
#include <iostream>
#include <sstream>
#include <any>
#include <algorithm>
#include <cctype>
#include <stdexcept>

inline unsigned int getWordIndex(const std::string& Str, unsigned int& From) {
    From = Str.find_first_not_of(" ,", From);
    if (From == std::string::npos) return INT_MAX;

    unsigned int found = Str.find_first_of(" ,", From);
    if (found == std::string::npos) return static_cast<unsigned int>(Str.size());

    if (found == From && Str[From] == ',') {
        return From + 1;
    }

    return found;
}

inline std::string getWord(const std::string& Str, unsigned int& From) {
    if (From == std::string::npos || From >= Str.size()) return "--1";

    unsigned int Last = getWordIndex(Str, From);
    if (Last == std::string::npos || Last == UINT_MAX) return "--1";

    std::string word = Str.substr(From, Last - From);

    From = Last;
    return word;
}

inline std::vector<std::string> getWordArray(const std::string& Str) {
    unsigned int From = 0;
    std::string word = "";
    std::vector<std::string> Words;
    while (true) {
        word = getWord(Str, From);
        if (word == "--1" || From >= Str.size()) break;
        Words.push_back(word);
    }

    return Words;
}


inline size_t findVar(std::string& str, size_t& start) {
    start = str.find_first_of("%$@", start);
    if (start == std::string::npos || start >= str.size()) return std::string::npos;
    size_t last = str.find_first_of(" */%$+-=;@'\"", start + 1);
    if (last == std::string::npos) return str.size();
    return last;
}

inline std::string ExtractPhrase(std::string& querry, std::string value) {
    size_t find = querry.find(value);
    if (find == std::string::npos) return "";

    size_t start = find + value.size();
    size_t end = querry.find('&', start);
    return (end == std::string::npos) ? querry.substr(start) : querry.substr(start, end - start);
};

// Helper to trim whitespace around parsed values
inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Splits a comma-separated string into a vector of trimmed tokens
inline std::vector<std::string> splitComma(const std::string& s) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        tokens.push_back(trim(item));
    }
    return tokens;
}

// Extracts a generic parameter value (e.g., Func=DumbJson)
inline std::string ExtractParam(const std::string& query, const std::string& key) {
    std::string searchKey = key;
    if (searchKey.back() != '=') searchKey += "=";

    size_t startPos = query.find(searchKey);
    if (startPos == std::string::npos) return "";
    startPos += searchKey.length();

    size_t endPos = query.find('&', startPos);
    if (endPos == std::string::npos) {
        return trim(query.substr(startPos));
    }
    return trim(query.substr(startPos, endPos - startPos));
}

// Extracts the content between { and } for blocks like para={...} or type={...}
inline std::string ExtractBlockValue(const std::string& query, const std::string& key) {
    std::string searchKey = key + "={";
    size_t startPos = query.find(searchKey);
    if (startPos == std::string::npos) return "";
    startPos += searchKey.length();

    size_t endPos = query.find('}', startPos);
    if (endPos == std::string::npos) return "";

    return query.substr(startPos, endPos - startPos);
}

bool parseBool(std::string s) {
    // Trim leading/trailing whitespace
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))  s.pop_back();

    // Lowercase for case-insensitive matching ("True", "TRUE", "true")
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    if (s == "true" || s == "1" || s == "yes") return true;
    if (s == "false" || s == "0" || s == "no") return false;

    throw std::runtime_error("Invalid boolean string: [" + s + "]");
}


inline std::vector<std::any> ExtractPara(std::string& query) {
    std::vector<std::any> params;

    std::string rawPara = ExtractBlockValue(query, "para");
    std::string rawType = ExtractBlockValue(query, "type");

    if (rawPara.empty() || rawType.empty()) return params;

    std::vector<std::string> valTokens = splitComma(rawPara);
    std::vector<std::string> typeTokens = splitComma(rawType);

    if (valTokens.size() != typeTokens.size()) {
        throw std::runtime_error("Parameter count and type count mismatch!");
    }

    for (size_t i = 0; i < valTokens.size(); ++i) {
        const std::string& val = valTokens[i];
        const std::string& type = typeTokens[i];


        if (type == "int") {
            params.push_back(std::stoi(val));
        }
        else if (type == "float") {
            params.push_back(std::stof(val));
        }
        else if (type == "double") {
            params.push_back(std::stod(val));
        }
        else if (type == "string") {
            params.push_back(val);
        }
        else if (type == "bool") {
            params.push_back(parseBool(val));
        }
        else {
            params.push_back(val);
        }

        //std::cout << val << "\n";
    }

    return params;
}


inline std::string anyToString(const std::any& a) {
    if (!a.has_value()) return "";
    if (a.type() == typeid(int))         return std::to_string(std::any_cast<int>(a));
    if (a.type() == typeid(float))       return std::to_string(std::any_cast<float>(a));
    if (a.type() == typeid(double))      return std::to_string(std::any_cast<double>(a));
    if (a.type() == typeid(std::string)) return std::any_cast<const std::string&>(a);
    if (a.type() == typeid(const char*)) return std::string(std::any_cast<const char*>(a));
    return "<unsupported type: " + std::string(a.type().name()) + ">";
}

inline void Replace(std::string& Str, std::string From, std::string To) {
    size_t pos = 0;

    while ((pos = Str.find(From, pos)) != std::string::npos) {
        Str.replace(Str.begin() + pos, Str.begin() + pos + From.size(), To);
        pos += To.size();
    }

    return;
}
