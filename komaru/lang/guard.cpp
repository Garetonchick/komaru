#include "guard.hpp"

#include <format>

namespace komaru::lang {

Guard::Guard(MorphismPtr morphism)
    : morphism_(std::move(morphism)) {
    if (morphism_->GetTarget() != Type::Bool()) {
        throw std::runtime_error(std::format("expected bool for guard's target but got {}",
                                             morphism_->GetTarget().ToString()));
    }
}

const Morphism& Guard::GetMorphism() const {
    return *morphism_;
}

std::string Guard::ToString(MorphismPtr arg_morphism, Style style) const {
    auto binded = Morphism::Binded(morphism_, {{0, arg_morphism}});

    switch (style) {
        case Style::Komaru:
            return "| " + binded->ToString();
        case Style::Haskell:
            return binded->ToString();
        case Style::Debug:
            return "Guard{" + binded->ToString() + "}";
    }
}

}  // namespace komaru::lang
