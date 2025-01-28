#include "type_registry.hpp"

namespace komaru::lang {

const Type* TypeRegistry::RegisterAtomType(TypeTag type) {
    types_.emplace_back(AtomType(type));
    return &types_.back();
}

const Type* TypeRegistry::RegisterTupleType(std::vector<const Type*> inner_types) {
    types_.emplace_back(TupleType(std::move(inner_types)));
    return &types_.back();
}

size_t TypeRegistry::GetNumTypes() const {
    return types_.size();
}

const Type* TypeRegistry::GetType(size_t idx) const {
    return &types_[idx];
}

}
