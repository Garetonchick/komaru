#include "type_registry.hpp"

namespace komaru::lang {

// const Type* TypeRegistry::RegisterAtomType(TypeTag type) {
//     types_.emplace_back(AtomType(type));
//     return &types_.back();
// }

// const Type* TypeRegistry::RegisterTupleType(std::vector<const Type*> inner_types) {
//     types_.emplace_back(TupleType(std::move(inner_types)));
//     return &types_.back();
// }

// size_t TypeRegistry::GetNumTypes() const {
//     return types_.size();
// }

// const Type* TypeRegistry::GetTypeByIdx(size_t idx) const {
//     return &types_[idx];
// }

// const Type* TypeRegistry::GetTypeByTag(TypeTag tag) const {
//     for(size_t i = 0; i < types_.size(); ++i) {
//         if(GetTypeTag(&types_[i]) == tag) {
//             return &types_[i];
//         }
//     }
//     return nullptr;
// }

// std::unique_ptr<TypeRegistry> MakeDefaultTypeRegistry() {
//     auto type_registry = std::make_unique<TypeRegistry>();

//     type_registry->RegisterAtomType(TypeTag::Bool);
//     const Type* ti = type_registry->RegisterAtomType(TypeTag::Int);
//     type_registry->RegisterAtomType(TypeTag::Char);
//     type_registry->RegisterAtomType(TypeTag::Float);
//     type_registry->RegisterTupleType({ti ,ti});

//     return type_registry;
// }

}  // namespace komaru::lang
