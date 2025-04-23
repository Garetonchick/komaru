#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <komaru/translate/program.hpp>
#include <komaru/translate/cpp/cpp_function.hpp>

namespace komaru::translate::cpp {

class CppProgramBuilder {
public:
    CppProgramBuilder() = default;

    void AddHeader(const std::string& header_name);
    void AddFunction(CppFunction func);
    void AddIncludeDir(const std::string& path);

    std::unique_ptr<IProgram> ExtractProgram();
    void Reset();

private:
    std::unordered_set<std::string> headers_;
    std::vector<CppFunction> funcs_;
    std::vector<std::string> order_;
    std::vector<std::string> include_dirs_;
};

}  // namespace komaru::translate::cpp
