#pragma once

#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/translator.hpp>
#include <komaru/translate/haskell/hs_import.hpp>

#include <vector>

namespace komaru::translate::hs {

using ResultProgram = TranslationResult<std::unique_ptr<IProgram>>;

class HaskellTranslator : public ITranslator {
public:
    explicit HaskellTranslator(std::vector<std::string> packages = {},
                               std::vector<HaskellImport> imports = {});

    ResultProgram Translate(const lang::CatProgram& cat_prog) override;

private:
    std::vector<std::string> packages_;
    std::vector<HaskellImport> imports_;
};

}  // namespace komaru::translate::hs
