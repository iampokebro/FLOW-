#pragma once
#include <iostream>
#include <any>
#include <functional>
#include <unordered_map>
#include <vector>
#include <utility>
#include <type_traits>
#include <string>

template<typename T>
T safeAnyCast(const std::any& a, const char* context) {
    try {
        return std::any_cast<T>(a);
    }
    catch (const std::bad_any_cast&) {
        const char* actual = a.has_value() ? a.type().name() : "<empty>";
        throw std::runtime_error(
            std::string("bad_any_cast in ") + context +
            ": expected [" + typeid(T).name() + "], got [" + actual + "]"
        );
    }
}

class ManagerBase {
public:
    ManagerBase() = default;

private:
    using Function = std::function<std::any(const std::vector<std::any>&)>;
    std::unordered_map<std::string, Function> _Handler;

    template<typename RetType, typename... Args, size_t... Is>
    std::any unpackCall(RetType(*Func)(Args...), const std::vector<std::any>& args, std::index_sequence<Is...>) {
        if (args.size() < sizeof...(Args)) {
            throw std::runtime_error("Not enough arguments passed to function! Expected " +
                std::to_string(sizeof...(Args)) + ", got " + std::to_string(args.size()));
        }
        if constexpr (std::is_void_v<RetType>) {
            Func(safeAnyCast<std::decay_t<Args>>(args[Is], "unpackCall arg")...);
            return {};
        }
        else {
            return std::any(Func(safeAnyCast<std::decay_t<Args>>(args[Is], "unpackCall arg")...));
        }
    }

public:
    template<typename Ret, typename... Args>
    void Register(const std::string& FuncName, Ret(*Func)(Args...)) {
        _Handler[FuncName] = [this, Func](const std::vector<std::any>& args) -> std::any {
            return unpackCall(Func, args, std::index_sequence_for<Args...>{});
            };
    }

    template<typename RetType>
    RetType Execute(const std::string& FuncName, const std::vector<std::any>& args) {
        auto it = _Handler.find(FuncName);
        if (it == _Handler.end()) {
            std::cout << FuncName << " No Such Function Registered\n";
            return RetType{};
        }

        // Properly cast the std::any return value to RetType
        return std::any_cast<RetType>(it->second(args));
    }

    std::any Execute(const std::string& FuncName, const std::vector<std::any>& args) {
        auto it = _Handler.find(FuncName);
        if (it == _Handler.end()) {
            std::cout << FuncName << " No Such Function Registered\n";
            return {};
        }

        return it->second(args);
    }
};