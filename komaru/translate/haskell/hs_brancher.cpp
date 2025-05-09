#include "hs_brancher.hpp"

namespace komaru::translate::hs {

TranslationResult<HaskellBrancher> HaskellBrancher::Make(std::string name, lang::Type type,
                                                         Brancher brancher) {
    // TODO: some checks
    return HaskellBrancher(std::move(name), std::move(type), std::move(brancher));
}

std::string HaskellBrancher::ToString() const {
    return std::visit(
        [](const auto& brancher) -> std::string {
            return brancher.ToString();
        },
        brancher_);
}

HaskellBrancher::HaskellBrancher(std::string name, lang::Type type, Brancher brancher)
    : name_(std::move(name)),
      type_(std::move(type)),
      brancher_(std::move(brancher)) {
}

}  // namespace komaru::translate::hs
