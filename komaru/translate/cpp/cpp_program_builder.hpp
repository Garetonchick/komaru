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
    bool HasFunction(const std::string& name) const;

    void AddHeader(const std::string& header_name);
    bool AddFunction(std::string mname, CppFunction func);

    std::unique_ptr<IProgram> ExtractProgram() const &&;

private:
    std::unordered_set<std::string> headers_;
    std::unordered_map<std::string, CppFunction> name2func_;
    std::vector<std::string> order_;
};

}
