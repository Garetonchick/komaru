#pragma once
#include <komaru/translate/program.hpp>

#include <string>
#include <vector>

namespace komaru::translate {

class CppProgram : public IProgram {
public:
    explicit CppProgram(std::string source_code, std::vector<std::string> include_dirs = {});

    const std::string& GetSourceCode() const override;
    const char* GetExt() const override;
    std::vector<std::string> GetBuildCommand(const std::string& filename,
                                             const std::string& outname) const override;

private:
    std::string source_code_;
    std::vector<std::string> include_dirs_;
};

}  // namespace komaru::translate
