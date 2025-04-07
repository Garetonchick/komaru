#include "cpp_program_builder.hpp"

#include <translate/cpp/cpp_program.hpp>

#include <format>

namespace komaru::translate::cpp {

bool CppProgramBuilder::HasFunction(const std::string& name) const {
    return name2func_.contains(name);
}

void CppProgramBuilder::AddHeader(const std::string& header_name) {
    headers_.insert(header_name);
}

bool CppProgramBuilder::AddFunction(std::string mname, CppFunction func) {
    order_.push_back(mname);
    auto [_, inserted] = name2func_.emplace(std::move(mname), func);
    return inserted;
}

std::unique_ptr<IProgram> CppProgramBuilder::ExtractProgram() const && {
    std::string source_code;

    for(const auto& header : headers_) {
        source_code += std::format("#include <{}>\n", header);
    }

    source_code += "\n\n";

    for(const auto& name : order_) {
        const auto& f = name2func_.at(name);

        source_code += f.decl;
        source_code += "\n";
    }

    source_code += "\n\n";

    for(const auto& name : order_) {
        const auto& f = name2func_.at(name);

        source_code += f.impl;
        source_code += "\n\n";
    }

    return std::make_unique<CppProgram>(std::move(source_code));
}

}
