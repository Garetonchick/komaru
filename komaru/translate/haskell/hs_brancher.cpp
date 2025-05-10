#include "hs_brancher.hpp"

#include <komaru/util/std_extensions.hpp>

namespace komaru::translate::hs {

TranslationResult<HaskellBrancher> HaskellBrancher::Make(std::string name, lang::Type type,
                                                         Brancher brancher) {
    // TODO: some checks
    return HaskellBrancher(std::move(name), std::move(type), std::move(brancher));
}

std::string HaskellBrancher::ToString(lang::MorphismPtr arg_morphism) const {
    return std::visit(util::Overloaded{
                          [&](const lang::Pattern& pattern) -> std::string {
                              return pattern.ToString(lang::Style::Haskell) + " <- " +
                                     arg_morphism->ToString();
                          },
                          [&](const lang::Guard& guard) -> std::string {
                              return guard.ToString(std::move(arg_morphism), lang::Style::Haskell);
                          },
                      },
                      brancher_);
}

HaskellBrancher::HaskellBrancher(std::string name, lang::Type type, Brancher brancher)
    : name_(std::move(name)),
      type_(std::move(type)),
      brancher_(std::move(brancher)) {
}

}  // namespace komaru::translate::hs
