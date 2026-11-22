#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <atomic>
#include <cstdlib>

#include <windows.h>
#include "../ManagerBase/ManagerBase.h"

extern std::string DllFolder;

struct FlyCompiler {
private:
    inline static std::atomic<int> s_compilationCounter{ 0 };
    std::vector<HMODULE> m_loadedModules;

    // Helper using native Win32 API to find clang++ on the system PATH
    std::string findClangBinary() {
        char buffer[MAX_PATH];
        DWORD result = SearchPathA(nullptr, "clang++", ".exe", MAX_PATH, buffer, nullptr);
        if (result != 0) {
            return std::string(buffer);
        }
        result = SearchPathA(nullptr, "clang", ".exe", MAX_PATH, buffer, nullptr);
        if (result != 0) {
            return std::string(buffer);
        }
        return "";
    }

public:
    ~FlyCompiler() {}

    bool compileWithClangDriver(const std::string& sourceCode, const std::string& outputDllPath, const std::string& tempSrcPath) {
        // 1. Write source code to a temporary file on disk
        {
            std::ofstream outFile(tempSrcPath);
            if (!outFile.is_open()) {
                std::cerr << "Failed to create temporary source file: " << tempSrcPath << "\n";
                return false;
            }
            outFile << sourceCode;
        }

        // 2. Locate clang++ binary using native Windows path search
        std::string clangBinaryPath = "clang++";

        // 3. Construct the compiler command line execution string
        // e.g., "C:\path\to\clang++.exe" -shared -O3 temp.cpp -o output.dll
        std::string command = clangBinaryPath + " -std=c++20 -shared -O3 " + tempSrcPath + " -o " + outputDllPath;
        
        // 4. Execute the compilation process
        int result = std::system(command.c_str());

        // Clean up temporary source file
        std::remove(tempSrcPath.c_str());
        
        if (result != 0) {
            std::cerr << "Clang compilation failed with exit code: " << result << "\n";
            return false;
        }

        return true;
    }

    bool compileFunction(ManagerBase& MB, std::string& FnName, std::string& GroupName, std::string& sourceCode) {
        typedef void (*fn)();

        int id = ++s_compilationCounter;
        std::string libPath = DllFolder + GroupName + "_" + std::to_string(id) + ".dll";
        std::string tempSrcPath = DllFolder + GroupName + "_" + std::to_string(id) + ".cpp";

        if (!compileWithClangDriver(sourceCode, libPath, tempSrcPath)) {
            return false;
        }

        HMODULE handle = LoadLibraryA(libPath.c_str());
        if (!handle) {
            std::cerr << "Failed to load generated DLL: " << libPath << " (Error code: " << GetLastError() << ")\n";
            return false;
        }

        m_loadedModules.push_back(handle);

        fn Func = reinterpret_cast<fn>(GetProcAddress(handle, FnName.c_str()));
        if (!Func) {
            std::cerr << "Failed to find symbol: " << FnName << " inside " << libPath << "\n";
            return false;
        }
        
        MB.Register(FnName, Func);
        return true;
    }
};