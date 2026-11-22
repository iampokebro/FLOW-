#pragma once
#include <fstream>
#include <filesystem>
#include <utility>
#include <sstream>
#include <array>

struct FunctionData {
    std::string Name = "";
    std::string Type = "";
    std::string Access = "";//private or publib
    std::vector<std::array<std::string, 3>> Parameter = {{"type", "name", "mutable"}};
    std::string Description = "";
};

inline void FunctionJson(FunctionData& FD, std::string& SaveFile) {

    std::filesystem::path filePath(SaveFile);
    if (filePath.has_parent_path() && !std::filesystem::exists(filePath.parent_path()) ) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    std::ofstream file(SaveFile, std::ios::app);


    file << ",{\n";
    file << "Kind : Function,\n";
    file << "Name : " << FD.Name << ",\n";
    file << "Type : " << FD.Type << ",\n";
    file << "Access : " << FD.Access << ",\n";
    file << "Parameter : [" << "\n";

    for (unsigned int i = 0; i < FD.Parameter.size(); i++) {
        std::array<std::string, 3>& Var = FD.Parameter[i];

        file << "{\n";
        file << "   Type : " << Var[0] << ",\n";
        file << "   Name : " << Var[1] << ",\n";
        file << "   Constant  : " << Var[2] << "\n";
        if (i == FD.Parameter.size() - 1) file << "}\n";
        else file << "},\n";
    }

    file << "],\n";
    file << "Description : " << FD.Description << "\n";
    file << "}\n";
    file.close();
}

inline void FileRemoveComma(std::string& SaveFile) {
    std::ifstream inFile(SaveFile);
    if (!inFile.is_open()) {
        std::cout << "!Open File, Comma Remove, Abstract Loader : " << SaveFile << "\n";
        return;
    }

    std::stringstream Buffer;
    Buffer << inFile.rdbuf();
    inFile.close();
    std::string buf = Buffer.str();

    size_t FStart = buf.find_first_not_of(" \t\n\r");
    if (FStart != std::string::npos && buf[FStart] == ',') {
        buf.erase(FStart, 1);
    }

    std::ofstream File(SaveFile, std::ios::trunc);
    if (!File.is_open()) {
        std::cout << "!Modify File, Comma Remove, Abstract Loader : " << SaveFile << "\n";
    }
    File << buf;
    File.close();
}

inline void FileClean(std::string& FileName) {
    if (!std::filesystem::exists(FileName)) return;
    std::ofstream file(FileName, std::ios::trunc);
    file.close();
    return;
}
