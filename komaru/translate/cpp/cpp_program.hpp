#pragma once
#include <translate/program.hpp>

#include <string>

namespace komaru::translate {

class CppProgram : public IProgram {
public:
    explicit CppProgram(std::string source_code);

    const std::string& GetSourceCode() const override;
    const char* GetExt() const override;
    std::vector<std::string> GetBuildCommand(const std::string& filename,
                                             const std::string& outname) const override;

private:
    std::string source_code_;
};

}  // namespace komaru::translate
