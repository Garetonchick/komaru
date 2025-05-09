#pragma once

#include <string>
#include <vector>

#include <komaru/translate/program.hpp>
#include <komaru/translate/haskell/hs_definition.hpp>

namespace komaru::translate::hs {

class HaskellProgramBuilder {
    struct Import {
        std::string module_name;
        std::string ref_name;
        std::vector<std::string> symbols;
    };

public:
    HaskellProgramBuilder() = default;

    void AddImport(std::string module_name, std::vector<std::string> symbols = {});
    void AddQualifiedImport(std::string module_name, std::string ref_name,
                            std::vector<std::string> symbols = {});
    void AddPragma(std::string pragma);
    const HaskellDefinition* AddDefinition(HaskellDefinition definition);
    void ChangeDefinitionName(const std::string& old_name, std::string new_name);

    std::unique_ptr<IProgram> Extract();

private:
    std::deque<HaskellDefinition> definitions_;
    std::vector<Import> imports_;
    std::vector<std::string> pragmas_;
};

}  // namespace komaru::translate::hs
