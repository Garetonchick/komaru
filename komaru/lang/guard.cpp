#include "guard.hpp"

#include <format>

namespace komaru::lang {

Guard::Guard(MorphismPtr morphism)
    : morphism_(std::move(morphism)) {
    if (morphism_->GetTarget() != Type::Bool()) {
        throw std::runtime_error(std::format("expected bool for guard's target but got {}",
                                             morphism_->GetTarget().GetName()));
    }
}

const Morphism& Guard::GetMorphism() const {
    return *morphism_;
}

std::string Guard::ToString() const {
    return "| " + morphism_->ToString();
}

}  // namespace komaru::lang
