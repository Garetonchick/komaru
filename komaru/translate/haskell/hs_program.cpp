#include "hs_program.hpp"

#include <vector>

namespace komaru::translate::hs {

HaskellProgram::HaskellProgram(std::string source_code)
    : source_code_(std::move(source_code)) {
}

const std::string& HaskellProgram::GetSourceCode() const {
    return source_code_;
}

const char* HaskellProgram::GetExt() const {
    return ".hs";
}

std::vector<std::string> HaskellProgram::GetBuildCommand(const std::string& filename,
                                                         const std::string& outname) const {
    return {"ghc", filename, "-o", outname};
}

}  // namespace komaru::translate::hs
