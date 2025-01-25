#pragma once
#include <lang/type.hpp>
#include <util/non_copyable_non_movable.hpp>

#include <deque>

namespace komaru::lang {

class TypeRegistry : util::NonCopyableNonMovable {
public:
    TypeRegistry() = default;
    ~TypeRegistry() = default;

    const Type& RegisterType(std::string name, BuiltinType type);
    const Type& RegisterType(
        std::string name,
        BuiltinType base_type,
        std::vector<const Type*> inner_types
    );

    // TODO: rework interface
    // For now simplistic golang-like interface
    size_t GetNumTypes() const;
    const Type& GetType(size_t idx) const;

private:
    std::deque<Type> types_;
};

}
