#pragma once
#include <komaru/translate/cpp/cpp_function.hpp>
#include <komaru/lang/type.hpp>

namespace komaru::translate::cpp {

class CppFunctionBuilder {
public:
    CppFunctionBuilder() = default;

    CppFunctionBuilder&& SetName(std::string name);
    CppFunctionBuilder&& SetReturnType(lang::Type type);
    CppFunctionBuilder&& AddInputParameter(lang::Type type, std::string name);
    CppFunctionBuilder&& SetBody(const std::string& body);

    CppFunction Extract() &&;

private:
    std::string name_;
    std::string body_;

    lang::Type ret_type_{lang::Type::Auto()};
    std::vector<std::pair<lang::Type, std::string>> input_params_;
};

}  // namespace komaru::translate::cpp
