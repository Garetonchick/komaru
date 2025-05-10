#pragma once

#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/translator.hpp>

namespace komaru::translate::hs {

class HaskellBrancher {
    using Brancher = lang::CatProgram::OutPin::Brancher;

public:
    static TranslationResult<HaskellBrancher> Make(std::string name, lang::Type type,
                                                   Brancher brancher);

    std::string ToString(lang::MorphismPtr arg_morphism) const;

private:
    HaskellBrancher(std::string name, lang::Type type, Brancher brancher);

private:
    std::string name_;
    lang::Type type_;
    Brancher brancher_;
};

}  // namespace komaru::translate::hs
