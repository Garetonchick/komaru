#include "cpp_program_builder.hpp"

#include <translate/cpp/cpp_program.hpp>

#include <format>

namespace komaru::translate::cpp {

void CppProgramBuilder::AddHeader(const std::string& header_name) {
    headers_.insert(header_name);
}

void CppProgramBuilder::AddFunction(CppFunction func) {
    funcs_.emplace_back(std::move(func));
}

std::unique_ptr<IProgram> CppProgramBuilder::ExtractProgram() {
    std::string source_code;

    for (const auto& header : headers_) {
        source_code += std::format("#include <{}>\n", header);
    }

    source_code += "\n\n";

    for (const auto& func : funcs_) {
        source_code += func.decl;
        source_code += "\n";
    }

    source_code += "\n\n";

    for (const auto& func : funcs_) {
        source_code += func.impl;
        source_code += "\n\n";
    }

    Reset();

    return std::make_unique<CppProgram>(std::move(source_code));
}

void CppProgramBuilder::Reset() {
    this->~CppProgramBuilder();
    new (this) CppProgramBuilder();
}

}  // namespace komaru::translate::cpp
