#include "morphism_registry.hpp"

namespace komaru::lang {

// const Morphism* MorphismRegistry::RegisterMorphism(
//     MorphismTag tag, const Type* source, const Type* target
// ) {
//     return &morphisms_.emplace_back(Morphism(tag, source, target));
// }
// const Morphism* MorphismRegistry::RegisterMorphism(
//     std::string name, std::vector<const Morphism*> morphisms
// ) {
//     return &morphisms_.emplace_back(
//         Morphism(std::move(name), std::move(morphisms))
//     );
// }

// size_t MorphismRegistry::GetNumMorphisms() const {
//     return morphisms_.size();
// }

// const Morphism* MorphismRegistry::GetMorphism(size_t idx) const {
//     return &morphisms_[idx];
// }

}
