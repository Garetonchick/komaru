#pragma once
#include <string>

namespace komaru::translate {

struct IProgram {
    virtual ~IProgram() = default;

    virtual const std::string& GetProgramSourceCode() = 0;
    virtual std::string GetProgramExt() = 0;
    virtual std::vector<std::string> GetBuildCommand(
        const std::string& filename,
        const std::string& outname
    ) = 0;
};

}
