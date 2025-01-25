#include "type_registry.hpp"

namespace komaru::lang {

const Type& TypeRegistry::RegisterType(std::string name, BuiltinType type) {
    types_.emplace_back(Type(std::move(name), type));
    return types_.back();
}

const Type& TypeRegistry::RegisterType(
    std::string name,
    BuiltinType base_type,
    std::vector<const Type*> inner_types
) {
    types_.emplace_back(Type(std::move(name), base_type, inner_types));
    return types_.back();
}

size_t TypeRegistry::GetNumTypes() const {
    return types_.size();
}

const Type& TypeRegistry::GetType(size_t idx) const {
    return types_[idx];
}

}
