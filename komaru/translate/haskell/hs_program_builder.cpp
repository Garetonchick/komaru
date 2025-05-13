#include "hs_program_builder.hpp"

#include <komaru/translate/haskell/hs_program.hpp>
#include <komaru/util/std_extensions.hpp>

#include <sstream>

namespace komaru::translate::hs {

void HaskellProgramBuilder::AddImport(HaskellImport import) {
    imports_.push_back(std::move(import));
}

void HaskellProgramBuilder::AddImport(std::string module_name, std::vector<std::string> symbols) {
    imports_.emplace_back(HaskellImport{
        .module_name = std::move(module_name),
        .ref_name = "",
        .symbols = std::move(symbols),
    });
}

void HaskellProgramBuilder::AddQualifiedImport(std::string module_name, std::string ref_name,
                                               std::vector<std::string> symbols) {
    imports_.emplace_back(HaskellImport{
        .module_name = std::move(module_name),
        .ref_name = std::move(ref_name),
        .symbols = std::move(symbols),
    });
}

void HaskellProgramBuilder::AddPragma(std::string pragma) {
    pragmas_.push_back(std::move(pragma));
}

void HaskellProgramBuilder::AddPackage(std::string package) {
    packages_.emplace_back(std::move(package));
}

const HaskellDefinition* HaskellProgramBuilder::AddDefinition(HaskellDefinition definition) {
    return &definitions_.emplace_back(std::move(definition));
}

void HaskellProgramBuilder::ChangeDefinitionName(const std::string& old_name,
                                                 std::string new_name) {
    for (auto& definition : definitions_) {
        if (definition.GetName() == old_name) {
            definition.ChangeName(std::move(new_name));
            return;
        }
    }
}

std::unique_ptr<IProgram> HaskellProgramBuilder::Extract() {
    std::stringstream ss;

    for (const auto& pragma : pragmas_) {
        ss << "{-# LANGUAGE " << pragma << " #-}\n";
    }

    ss << "\n";

    for (const auto& import : imports_) {
        ss << import.ToString() << "\n";
    }

    ss << "\n";

    for (const auto& definition : definitions_) {
        ss << definition.ToString() << "\n\n";
    }

    return std::make_unique<HaskellProgram>(ss.str(), std::move(packages_));
}

}  // namespace komaru::translate::hs
