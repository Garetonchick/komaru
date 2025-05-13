#pragma once

#include <komaru/translate/program.hpp>

#include <vector>

namespace komaru::translate::hs {

class HaskellProgram : public IProgram {
public:
    explicit HaskellProgram(std::string source_code, std::vector<std::string> packages);

    const std::string& GetSourceCode() const override;
    const char* GetExt() const override;
    std::vector<std::string> GetBuildCommand(const std::string& filename,
                                             const std::string& outname) const override;

private:
    std::string source_code_;
    std::vector<std::string> packages_;
};

}  // namespace komaru::translate::hs
