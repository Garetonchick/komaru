#pragma once
#include <komaru/lang/type.hpp>

#include <string>

namespace komaru::translate::cpp {

class CppType {
public:
    CppType(std::string type_str, std::vector<std::string> template_vars);

    const std::string& GetTypeStr() const;
    const std::vector<std::string>& GetTemplateVars() const;

private:
    std::string type_str_;
    std::vector<std::string> template_vars_;
};

CppType ToCppType(lang::Type type);

}  // namespace komaru::translate::cpp
