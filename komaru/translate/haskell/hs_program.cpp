#include "hs_program.hpp"

#include <vector>

namespace komaru::translate::hs {

HaskellProgram::HaskellProgram(std::string source_code, std::vector<std::string> packages)
    : source_code_(std::move(source_code)),
      packages_(std::move(packages)) {
}

const std::string& HaskellProgram::GetSourceCode() const {
    return source_code_;
}

const char* HaskellProgram::GetExt() const {
    return ".hs";
}

std::vector<std::string> HaskellProgram::GetBuildCommand(const std::string& filename,
                                                         const std::string& outname) const {
    std::vector<std::string> command = {"ghc", filename, "-o", outname};

    for (const auto& package : packages_) {
        command.emplace_back("-package");
        command.emplace_back(package);
    }
    return command;
}

}  // namespace komaru::translate::hs
