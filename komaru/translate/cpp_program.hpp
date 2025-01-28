#pragma once
#include <translate/program.hpp>

#include <string>

namespace komaru::translate {

class CppProgram : public IProgram {
public:
    CppProgram(std::string source_code);

    const std::string& GetProgramSourceCode() override;
    std::string GetProgramExt() override;
    std::vector<std::string> GetBuildCommand(
        const std::string& filename,
        const std::string& outname
    ) override;

private:
    std::string source_code_;
};

}
