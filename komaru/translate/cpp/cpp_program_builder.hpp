#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <translate/program.hpp>
#include <translate/cpp/cpp_function.hpp>

namespace komaru::translate::cpp {

class CppProgramBuilder {
public:
    CppProgramBuilder() = default;

    void AddHeader(const std::string& header_name);
    void AddFunction(CppFunction func);

    std::unique_ptr<IProgram> ExtractProgram();
    void Reset();

private:
    std::unordered_set<std::string> headers_;
    std::vector<CppFunction> funcs_;
    std::vector<std::string> order_;
};

}  // namespace komaru::translate::cpp
