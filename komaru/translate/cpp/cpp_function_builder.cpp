#include "cpp_function_builder.hpp"

#include <util/string.hpp>
#include <translate/cpp/cpp_types.hpp>
#include <util/std_extensions.hpp>

#include <unordered_set>

namespace komaru::translate::cpp {

CppFunctionBuilder&& CppFunctionBuilder::SetName(std::string name) {
    name_ = std::move(name);
    return std::move(*this);
}

CppFunctionBuilder&& CppFunctionBuilder::SetReturnType(lang::Type type) {
    ret_type_ = type;
    return std::move(*this);
}

CppFunctionBuilder&& CppFunctionBuilder::AddInputParameter(lang::Type type, std::string name) {
    input_params_.emplace_back(type, std::move(name));
    return std::move(*this);
}

CppFunctionBuilder&& CppFunctionBuilder::SetBody(const std::string& body) {
    body_.clear();

    if (!body.empty()) {
        body_ += "    ";
    }

    for (char c : body) {
        body_.push_back(c);
        if (c == '\n') {
            body_ += "    ";
        }
    }
    return std::move(*this);
}

CppFunction CppFunctionBuilder::Extract() && {
    CppFunction result;

    std::unordered_set<std::string> template_vars;
    std::vector<std::string> param_strs;

    CppType cpp_ret_type = ToCppType(ret_type_);
    template_vars.insert(cpp_ret_type.GetTemplateVars().begin(),
                         cpp_ret_type.GetTemplateVars().end());

    for (const auto& [param_type, param_name] : input_params_) {
        CppType cpp_param_type = ToCppType(param_type);
        template_vars.insert(cpp_param_type.GetTemplateVars().begin(),
                             cpp_param_type.GetTemplateVars().end());
        param_strs.push_back(cpp_param_type.GetTypeStr() + " " + param_name);
    }

    if (!template_vars.empty()) {
        result.decl += "template<";

        for (const auto& [i, tvar] : util::Enumerate(template_vars)) {
            result.decl += "typename " + tvar;

            if (i + 1 != template_vars.size()) {
                result.decl += ", ";
            }
        }

        result.decl += ">\n";
    }

    result.decl += cpp_ret_type.GetTypeStr() + " ";
    result.decl += name_ + "(";

    for (const auto& [i, param_str] : util::Enumerate(param_strs)) {
        result.decl += param_str;

        if (i + 1 != param_strs.size()) {
            result.decl += ", ";
        }
    }

    result.decl += ")";
    result.impl = result.decl;
    result.decl += ";";

    result.impl += " {\n" + body_ + "\n}";

    return result;
}

}  // namespace komaru::translate::cpp
