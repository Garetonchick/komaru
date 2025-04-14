#pragma once
#include <komaru/lang/type.hpp>
#include <komaru/util/non_copyable_non_movable.hpp>

namespace komaru::lang {

// class TypeRegistry : util::NonCopyableNonMovable {
// public:
//     TypeRegistry() = default;
//     ~TypeRegistry() = default;

//     const Type* RegisterAtomType(TypeTag type);
//     const Type* RegisterTupleType(std::vector<const Type*> inner_types);

//     // TODO: rework interface
//     // For now simplistic golang-like interface
//     size_t GetNumTypes() const;
//     const Type* GetTypeByIdx(size_t idx) const;
//     const Type* GetTypeByTag(TypeTag tag) const;

// private:
//     std::deque<Type> types_;
// };

// std::unique_ptr<TypeRegistry> MakeDefaultTypeRegistry();

}  // namespace komaru::lang
