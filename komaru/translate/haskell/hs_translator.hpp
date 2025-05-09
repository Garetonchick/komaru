#pragma once

#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/translator.hpp>

namespace komaru::translate::hs {

using ResultProgram = TranslationResult<std::unique_ptr<IProgram>>;

class HaskellTranslator : public ITranslator {
public:
    HaskellTranslator();

    ResultProgram Translate(const lang::CatProgram& cat_prog) override;
};

}  // namespace komaru::translate::hs
