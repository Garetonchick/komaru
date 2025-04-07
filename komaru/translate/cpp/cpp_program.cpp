#include "cpp_program.hpp"

#include <vector>

namespace komaru::translate {

CppProgram::CppProgram(std::string source_code) : source_code_(std::move(source_code)) {
}

const std::string& CppProgram::GetSourceCode() const {
    return source_code_;
}

const char* CppProgram::GetExt() const {
    return ".cpp";
}

std::vector<std::string> CppProgram::GetBuildCommand(
    const std::string& filename,
    const std::string& outname
) const {
    return std::vector<std::string>{
        "$CXX",
        filename,
        "-o",
        outname
    };
}

}
