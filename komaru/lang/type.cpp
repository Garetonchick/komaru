#include "type.hpp"

namespace komaru::lang {

const std::string& Type::GetName() const {
    return name_;
}

BuiltinType Type::GetBaseType() const {
    return base_type_;
}

const std::vector<const Type*>& Type::GetInnerTypes() const {
    return inner_types_;
}

bool Type::IsSameAs(const Type& o) const {
    return this == &o;
}

Type::Type(std::string name, BuiltinType type)
    : name_(std::move(name)), base_type_(type) {
}

Type::Type(
    std::string name,
    BuiltinType base_type,
    std::vector<const Type*> inner_types
) : name_(std::move(name))
  , base_type_(base_type)
  , inner_types_(std::move(inner_types)) {
}

}
