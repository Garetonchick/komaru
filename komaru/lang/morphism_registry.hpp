#pragma once
#include <komaru/util/non_copyable_non_movable.hpp>
#include <komaru/lang/morphism.hpp>

#include <deque>

namespace komaru::lang {

// class MorphismRegistry : util::NonCopyableNonMovable {
// public:
//     MorphismRegistry() = default;
//     ~MorphismRegistry() = default;

//     const Morphism* RegisterMorphism(MorphismTag tag, const Type* source, const Type* target);
//     const Morphism* RegisterMorphism(std::string name, std::vector<const Morphism*> morphisms);

//     // TODO: rework interface
//     // For now simplistic golang-like interface
//     size_t GetNumMorphisms() const;
//     const Morphism* GetMorphism(size_t idx) const;

// private:
//     std::deque<Morphism> morphisms_;
// };

}  // namespace komaru::lang
