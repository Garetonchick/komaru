#include "cpp_program.hpp"

#include <vector>

namespace komaru::translate {

CppProgram::CppProgram(std::string source_code, std::vector<std::string> include_dirs)
    : source_code_(std::move(source_code)),
      include_dirs_(std::move(include_dirs)) {
}

const std::string& CppProgram::GetSourceCode() const {
    return source_code_;
}

const char* CppProgram::GetExt() const {
    return ".cpp";
}

std::vector<std::string> CppProgram::GetBuildCommand(const std::string& filename,
                                                     const std::string& outname) const {
    auto command = std::vector<std::string>{"$CXX", "-std=c++23", filename, "-o", outname};

    for (const auto& dir : include_dirs_) {
        command.push_back("-I" + dir);
    }

    return command;
}

}  // namespace komaru::translate
