#include "cpp_types.hpp"

namespace komaru::translate::cpp {

CppType::CppType(std::string type_str, std::vector<std::string> template_vars)
    : type_str_(std::move(type_str)), template_vars_(std::move(template_vars)) {}

const std::string& CppType::GetTypeStr() const {
    return type_str_;
}

const std::vector<std::string>& CppType::GetTemplateVars() const {
    return template_vars_;
}

static CppType TranslateType(const lang::AtomType& type) {
    switch(type.GetTag()) {
        case lang::TypeTag::Int:
            return CppType("int32_t", {});
        case lang::TypeTag::Bool:
            return CppType("bool", {});
        case lang::TypeTag::Char:
            return CppType("char", {});
        case lang::TypeTag::Float:
            return CppType("float", {});
        case lang::TypeTag::Singleton:
            return CppType("std::monostate", {});
        case lang::TypeTag::Source:
            return CppType("std::monostate", {});
        case lang::TypeTag::Target:
            return CppType("auto", {});
        default:
            throw std::runtime_error("unsupported cpp type");
    }
}

static CppType TranslateType(const lang::TupleType& type) {
    const auto& sub_types = type.GetTupleTypes();
    std::string type_str = "std::tuple<";
    std::vector<std::string> template_vars;

    for(size_t i = 0; i < sub_types.size(); ++i) {
        auto cpp_sub_type = ToCppType(sub_types[i]);

        const auto& sub_template_vars = cpp_sub_type.GetTemplateVars();
        template_vars.insert(
            template_vars.end(), sub_template_vars.begin(), sub_template_vars.end()
        );

        type_str += cpp_sub_type.GetTypeStr();

        if(i + 1 != sub_types.size()) {
            type_str += ", ";
        }
    }

    type_str += ">";

    return CppType(std::move(type_str), std::move(template_vars));
}

static CppType TranslateType(const lang::GenericType& type) {
    auto name = std::string(type.GetName());

    if(name.empty()) {
        return CppType("auto", {});
    }

    return CppType(name, {name});
}

CppType ToCppType(lang::Type type) {
    return type.Visit([](const auto& type) {
        return TranslateType(type);
    });
}

}
