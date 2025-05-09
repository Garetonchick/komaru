#include "cpp_types.hpp"

#include <komaru/util/std_extensions.hpp>

#include <format>

namespace komaru::translate::cpp {

static const std::unordered_map<lang::Type, CppType> kTypeMap = {
    {lang::Type::Int(), CppType("int32_t", {})},
    {lang::Type::Bool(), CppType("bool", {})},
    {lang::Type::Char(), CppType("char", {})},
    {lang::Type::Float(), CppType("float", {})},
    {lang::Type::Double(), CppType("double", {})},
    {lang::Type::Singleton(), CppType("std::monostate", {})},
};

CppType::CppType(std::string type_str, std::vector<std::string> template_vars)
    : type_str_(std::move(type_str)),
      template_vars_(std::move(template_vars)) {
}

const std::string& CppType::GetTypeStr() const {
    return type_str_;
}

const std::vector<std::string>& CppType::GetTemplateVars() const {
    return template_vars_;
}

static CppType TranslateType(const lang::CommonType& type) {
    auto type_str = type.GetName() + "<";
    std::vector<std::string> template_vars;

    if (!lang::IsConcreteTypeName(type.GetName())) {
        throw std::runtime_error("TODO: add template inside template support");
    }

    for (auto [i, param] : util::Enumerate(type.GetTypeParams())) {
        auto param_cpp = ToCppType(param);
        template_vars.append_range(param_cpp.GetTemplateVars());

        type_str += param_cpp.GetTypeStr();

        if (i + 1 != type.GetTypeParams().size()) {
            type_str += ", ";
        }
    }

    type_str += ">";
    return CppType(std::move(type_str), std::move(template_vars));
}

static CppType TranslateType(const lang::TupleType& type) {
    const auto& sub_types = type.GetTupleTypes();
    std::string type_str = "std::tuple<";
    std::vector<std::string> template_vars;

    for (size_t i = 0; i < sub_types.size(); ++i) {
        auto cpp_sub_type = ToCppType(sub_types[i]);

        const auto& sub_template_vars = cpp_sub_type.GetTemplateVars();
        template_vars.insert(template_vars.end(), sub_template_vars.begin(),
                             sub_template_vars.end());

        type_str += cpp_sub_type.GetTypeStr();

        if (i + 1 != sub_types.size()) {
            type_str += ", ";
        }
    }

    type_str += ">";

    return CppType(std::move(type_str), std::move(template_vars));
}

static CppType TranslateType(const lang::ListType& type) {
    auto cpp_sub_type = ToCppType(type.Inner());
    return CppType(std::format("std::vector<{}>", cpp_sub_type.GetTypeStr()),
                   cpp_sub_type.GetTemplateVars());
}

static CppType TranslateType(const lang::FunctionType&) {
    throw std::runtime_error("TODO: add function type support");
}

CppType ToCppType(lang::Type type) {
    auto it = kTypeMap.find(type);
    if (it != kTypeMap.end()) {
        return it->second;
    }

    return type.Visit([](const auto& type) -> CppType {
        return TranslateType(type);
    });
}

}  // namespace komaru::translate::cpp
