#pragma once

#include <komaru/translate/program.hpp>

namespace komaru::translate::hs {

class HaskellProgram : public IProgram {
public:
    explicit HaskellProgram(std::string source_code);

    const std::string& GetSourceCode() const override;
    const char* GetExt() const override;
    std::vector<std::string> GetBuildCommand(const std::string& filename,
                                             const std::string& outname) const override;

private:
    std::string source_code_;
};

}  // namespace komaru::translate::hs
