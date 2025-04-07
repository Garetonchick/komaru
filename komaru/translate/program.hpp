#pragma once
#include <string>

namespace komaru::translate {

struct IProgram {
    virtual ~IProgram() = default;

    virtual const std::string& GetSourceCode() const = 0;
    virtual const char* GetExt() const = 0;
    virtual std::vector<std::string> GetBuildCommand(
        const std::string& filename,
        const std::string& outname
    ) const = 0;
};

}
